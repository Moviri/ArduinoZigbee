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
#error Define ZB_ROUTER_ROLE to compile light bulb (Router) source code.
#endif

zb_af_device_ctx_t dev_ctx;
// zb_af_endpoint_desc_typed_t ep_list[4];
zb_af_endpoint_desc_t *ep_list_desc[4];

int current_size_ep = 0;

bool add_EP(zb_af_endpoint_desc_t* ep, ep_type_enum ep_type) {

    // ep_list[current_size_ep].ep = ep;
    // ep_list[current_size_ep].ep_type = ep_type;
    
    ep_list_desc[current_size_ep] = ep;
    current_size_ep++;
    return 1;
}

void init_device_ctx() {
    dev_ctx.ep_count = current_size_ep;
    dev_ctx.ep_desc_list = ep_list_desc;
}

static zb_void_t zcl_device_cb(zb_bufid_t bufid)
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

void register_endpoint_cb() {
    for(uint8_t i=0; i < current_size_ep; i++) {
        ZB_AF_SET_ENDPOINT_HANDLER(ep_list_desc[i]->ep_id, zcl_light_endpoint_cb);
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
