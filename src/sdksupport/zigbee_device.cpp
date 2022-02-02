#include "Zigbee.h"
extern "C" {
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "zigbee_device.h"
#include "endpoints/endpoint_enum.h"
#include "endpoints/dimmable_light/zigbee_dimmable_light.h"
#include "endpoints/temperature_sensor/zigbee_temperature_sensor.h"

#if !defined ZB_ROUTER_ROLE
#error Define ZB_ROUTER_ROLE to compile Router source code.
#endif

zb_af_device_ctx_t dev_ctx;


void init_device_ctx() {
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
    for(uint8_t i=0; i<endpoints().size(); i++) {
        if(endpoints()[i]->ep_id == ep_id) {
            return endpoints()[i];
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
