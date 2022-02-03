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
#include "zigbee_device.h"
#include "pal_bb.h"

#include "Arduino.h"
extern "C"
{
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "endpoints/zigbee_endpoint.h"
#include "sdksupport/zigbee_setup.h"

#if !defined ZB_ROUTER_ROLE
#error Define ZB_ROUTER_ROLE to compile Router source code.
#endif

extern "C"
{
    void nrf_802154_core_irq_handler(void);
}

/** Global library helper object reference typical of Arduino.
 * To be used only in the .ino file.
 */
ZigbeeDevice &ZIGBEE = ZigbeeDevice::getInstance();

static zb_void_t processZclDeviceCommand(zb_bufid_t bufid)
{
    zb_zcl_device_callback_param_t *p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);
    ZigbeeEndpoint *endpoint = ZigbeeDevice::getInstance().getEndpointByID(p_device_cb_param->endpoint);
    if (endpoint == nullptr)
    {
        p_device_cb_param->status = RET_ERROR;
        return;
    }
    p_device_cb_param->status = endpoint->processCommandDV(p_device_cb_param);
}

static zb_uint8_t processCommandCallback(zb_bufid_t bufid)
{
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);
    if (cmd_info == nullptr)
    {
        return ZB_FALSE;
    }

    ZigbeeEndpoint *endpoint = ZigbeeDevice::getInstance().getEndpointByID(cmd_info->addr_data.common_data.dst_endpoint);
    if (endpoint != nullptr)
    {
        if (endpoint->processCommandEP(bufid, cmd_info))
        {
            zb_buf_free(bufid);
            return ZB_TRUE;
        }
    }
    return ZB_FALSE;
}

ZigbeeEndpoint *ZigbeeDevice::getEndpointByID(uint8_t ep_id) const
{
    for (uint8_t i = 0; i < m_endpoints.size(); i++)
    {
        ZigbeeEndpoint *endpoint = m_endpoints[i];
        if (endpoint->endpointId() == ep_id)
        {
            return endpoint;
        }
    }
    return nullptr;
}

void ZigbeeDevice::initDeviceContext()
{
    m_dev_ctx.ep_count = m_endpoints.size();
    zb_af_endpoint_desc_t **ep_desc_arr = new zb_af_endpoint_desc_t *[m_dev_ctx.ep_count];

    for (int i = 0; i < m_endpoints.size(); i++)
    {
        ep_desc_arr[i] = m_endpoints[i]->endpointDescriptor();
    }
    m_dev_ctx.ep_desc_list = ep_desc_arr;
}

int ZigbeeDevice::initDevice()
{
    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(processZclDeviceCommand);

    /* Register dimmer switch device context (endpoints). */
    initDeviceContext();
    ZB_AF_REGISTER_DEVICE_CTX(&m_dev_ctx);

    /* register endpoints callback. */
    for (uint8_t i = 0; i < m_endpoints.size(); i++)
    {
        ZB_AF_SET_ENDPOINT_HANDLER(m_endpoints[i]->endpointId(), processCommandCallback);
    }

    return 1;
}

ZigbeeDevice &ZigbeeDevice::getInstance()
{
    // Instantiate just one instance on the first use.
    static ZigbeeDevice instance;
    return instance;
}

ZigbeeDevice::ZigbeeDevice() : m_zb_tc_key(nullptr), m_dev_ctx({0})
{
}

ZigbeeDevice::~ZigbeeDevice()
{
    end();
    if (m_dev_ctx.ep_desc_list != nullptr)
    {
        delete m_dev_ctx.ep_desc_list;
    }
}

void ZigbeeDevice::setTrustCenterKey(zb_uint8_t *zb_tc_key_l)
{
    m_zb_tc_key = zb_tc_key_l;
}

int ZigbeeDevice::begin(const zb_uint32_t channel)
{
    return begin(std::vector<unsigned int>{channel});
}

int ZigbeeDevice::begin(const std::vector<zb_uint32_t> channels)
{
    zb_uint32_t channel_mask = 0;
    for (int i = 0; i < channels.size(); i++)
    {
        if (channels[i] >= kFirstChannel && channels[i] <= kLastChannel)
        {
            channel_mask |= 1 << channels[i];
        }
    }

    PalBbSetProtId(BB_PROT_15P4);
    PalBbRegisterProtIrq(BB_PROT_15P4, NULL, nrf_802154_core_irq_handler);

    zigbee_init((channel_mask == 0) ? ZB_TRANSCEIVER_ALL_CHANNELS_MASK : channel_mask);
    if (m_zb_tc_key != nullptr)
    {
        zb_zdo_set_tc_standard_distributed_key(m_zb_tc_key);
        zb_zdo_setup_network_as_distributed();
    }

    initDevice();
    return (zigbee_start() == RET_OK) ? 0 : -1;
}

void ZigbeeDevice::end()
{
    /* Don't need to do anything special. The user can stop calling poll() at any time. */
}

void ZigbeeDevice::poll()
{
    zboss_main_loop_iteration();

    /* Check if periodic callbacks need to be triggered */
    updateEndpoints();
}

int ZigbeeDevice::addEndpoint(ZigbeeEndpoint &endpoint)
{
    m_endpoints.push_back(&endpoint);
    return 0;
}

void ZigbeeDevice::setDeviceName(zb_char_t model_id[])
{
    if (m_endpoints.size() > 0)
    {
        m_endpoints[0]->setModelID(model_id);
    }
}

void ZigbeeDevice::updateEndpoints()
{
    // CB are triggered period seconds after the previous call. Relative time, not absolute time.
    const uint32_t curr_time = millis();
    for (int i = 0; i < m_endpoints.size(); i++)
    {
        m_endpoints[i]->poll(curr_time);
    }
}
