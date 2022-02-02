/*
  This file is part of the ArduinoZigbee library.
  Copyright (c) 2021 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "Zigbee.h"
#include "pal_bb.h"

#include "Arduino.h"
#include "Zigbee.h"
extern "C" {
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "endpoints/endpoint_enum.h"
#include "endpoints/dimmable_light/zigbee_dimmable_light.h"
#include "endpoints/temperature_sensor/zigbee_temperature_sensor.h"

#if !defined ZB_ROUTER_ROLE
#error Define ZB_ROUTER_ROLE to compile Router source code.
#endif

extern "C"
{
    int zigbee_init(const zb_uint32_t channelMask);
    int zigbee_start();
    void nrf_802154_core_irq_handler(void);
}

/** Global library helper object reference typical of Arduino.
 * To be used only in the .ino file.
 */
Zigbee &ZIGBEE = Zigbee::getInstance();

zb_af_device_ctx_t dev_ctx;

static void init_device_ctx() {
    std::vector<EndpointCTX*>& endpoints = Zigbee::getInstance().endpoints();
    dev_ctx.ep_count = endpoints.size();
    zb_af_endpoint_desc_t** ep_desc_arr = new zb_af_endpoint_desc_t*[dev_ctx.ep_count];

    for(int i=0; i < endpoints.size(); i++) {
        ep_desc_arr[i] = endpoints[i]->ep_desc;
    }
    dev_ctx.ep_desc_list = ep_desc_arr;
}

static zb_void_t zcl_device_cb(zb_bufid_t bufid)  // TODO: review how single CB_ID are managed. We should avoid function specific to a particular device(light, thermostat,...)
{
    zb_uint8_t cluster_id;
    zb_uint8_t attr_id;
    zb_zcl_device_callback_param_t *p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    /* Set default response value. */
    p_device_cb_param->status = RET_OK;

    switch (p_device_cb_param->device_cb_id)
    {
    case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID: // Inform user about ZCL Level Control cluster attributes value modification
        zb_light_set_level(p_device_cb_param->endpoint, p_device_cb_param->cb_param.level_control_set_value_param.new_value);
        break;

    case ZB_ZCL_SET_ATTR_VALUE_CB_ID: // Inform user about attribute value modification
        zb_light_set_attribute(p_device_cb_param->endpoint, &p_device_cb_param->cb_param.set_attr_value_param);
        break;

    default:
        p_device_cb_param->status = RET_ERROR;
        break;
    }
}

zb_uint8_t endpoint_CB_wrapper(zb_bufid_t bufid) {
    zb_bufid_t zcl_cmd_buf = bufid;
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);
    uint8_t ep_id = cmd_info->addr_data.common_data.dst_endpoint;

    std::vector<EndpointCTX*>& endpoints = Zigbee::getInstance().endpoints();
    EndpointCTX* ctx;
    for(uint8_t i=0; i<endpoints.size(); i++) {
        if(endpoints[i]->ep_id == ep_id) {
            return endpoints[i]->endpoint_CB(bufid);
        }
    }
    return ZB_FALSE;
}

void register_endpoint_cb() {
    std::vector<EndpointCTX*>& endpoints = Zigbee::getInstance().endpoints();
    for(uint8_t i=0; i < endpoints.size(); i++) {
        ZB_AF_SET_ENDPOINT_HANDLER(endpoints[i]->ep_id, endpoint_CB_wrapper);
    }
}

int init_device() {
    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    /* Register dimmer switch device context (endpoints). */
    init_device_ctx();
    ZB_AF_REGISTER_DEVICE_CTX(&dev_ctx);

    register_endpoint_cb();

    return 1;
}

Zigbee& Zigbee::getInstance()
{
    // Instantiate just one instance on the first use.
    static Zigbee instance;
    return instance;
}

std::vector<EndpointCTX*>& Zigbee::endpoints() {
    return m_endpoints;
}

Zigbee::Zigbee(): m_zb_tc_key(nullptr)
{
}

Zigbee::~Zigbee()
{
}

void Zigbee::setTrustCenterKey(zb_uint8_t *zb_tc_key_l)
{
    m_zb_tc_key = zb_tc_key_l;
}

int Zigbee::begin(const zb_uint32_t channelMask)
{
    PalBbSetProtId(BB_PROT_15P4);
    PalBbRegisterProtIrq(BB_PROT_15P4, NULL, nrf_802154_core_irq_handler);
    zigbee_init(channelMask);
    if (m_zb_tc_key != nullptr) {
        zb_zdo_set_tc_standard_distributed_key(m_zb_tc_key);
        zb_zdo_setup_network_as_distributed();
    }
     
    init_device();
    zigbee_start();
}

void Zigbee::end()
{
}

void Zigbee::poll()
{
    zboss_main_loop_iteration();

    // Check if periodic callbacks need to be triggered
    Zigbee::check_periodic_CB();
}

int Zigbee::addEP(EndpointCTX* ep_ctx) 
{
    m_endpoints.push_back(ep_ctx);
    return 0;
}

// CB are triggered period seconds after the previous call. Relative time, not absolute time
void Zigbee::check_periodic_CB()
{
    uint32_t curr_time = millis();
    for(int i=0; i < m_endpoints.size(); i++) {
        EndpointCTX* endpoint = m_endpoints[i];
        if ((endpoint->period > 0) && (curr_time - endpoint->last_trig_time >= endpoint->period)) {
            endpoint->periodic_CB();
            endpoint->last_trig_time = curr_time;
        }
    }
}
