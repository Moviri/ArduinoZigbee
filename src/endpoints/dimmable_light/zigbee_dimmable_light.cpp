#include <mbed.h>
#include "zigbee_dimmable_light.h"

DimmableLightCTX::DimmableLightCTX(float period_p, void (*aLightCB)(const uint8_t brightness_level)) : EndpointCTX(period_p)
{
    lightCB = aLightCB;

    /* Basic cluster attributes data */
    basic_attr.app_version = 1;
    basic_attr.stack_version = 10;
    basic_attr.hw_version = 11;

    ZB_ZCL_SET_STRING_VAL(basic_attr.mf_name,
                          "Arduino",
                          ZB_ZCL_STRING_CONST_SIZE("Arduino"));

    ZB_ZCL_SET_STRING_VAL(basic_attr.model_id,
                          "Dimmable Light v1",
                          ZB_ZCL_STRING_CONST_SIZE("Dimmable Light v1"));

    ZB_ZCL_SET_STRING_VAL(basic_attr.date_code,
                          "20210911",
                          ZB_ZCL_STRING_CONST_SIZE("20210911"));

    basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE;

    ZB_ZCL_SET_STRING_VAL(basic_attr.location_id,
                          "Generic",
                          ZB_ZCL_STRING_CONST_SIZE("Generic"));

    basic_attr.ph_env = ZB_ZCL_BASIC_ENV_UNSPECIFIED;

    /* On/Off cluster attributes data */
    on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;

    /* Level control cluster attributes data */
    level_control_attr.current_level = ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE; // Set current level value to maximum
    level_control_attr.remaining_time = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;
}

zb_uint8_t DimmableLightCTX::endpoint_CB(zb_bufid_t bufid)
{
    zb_bufid_t zcl_cmd_buf = bufid;
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);

    if (    cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV &&
            !cmd_info->is_common_command &&
            cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
            cmd_info->cmd_id == ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID)
    {
        setState((zb_bool_t)false);

        zb_buf_free(bufid);

        return ZB_TRUE;
    }

    return ZB_FALSE;
}

void DimmableLightCTX::periodic_CB()
{
    /* Not used */
}

// static void zb_set_brightness(zb_uint8_t brightness_level, uint8_t ep_id)
// {
//     for (uint8_t i = 0; i < current_light_size; i++)
//     {
//         if (ctx_light_arr[i]->ep_id == ep_id) {
//             ctx_light_arr[i]->bulbCB(brightness_level);
//             break;
//         }
//     }
// }

// zb_uint8_t zcl_light_endpoint_cb(zb_bufid_t bufid) {
//     zb_bufid_t zcl_cmd_buf = bufid;
//     zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);

//     if (    cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV &&
//             !cmd_info->is_common_command &&
//             cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
//             cmd_info->cmd_id == ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID)
//     {
//         zb_light_set_state(cmd_info->addr_data.common_data.dst_endpoint, (zb_bool_t)false);

//         zb_buf_free(bufid);

//         return ZB_TRUE;
//     }

//     return ZB_FALSE;
// }


void DimmableLightCTX::setAttribute(zb_zcl_set_attr_value_param_t *attr_p)
{
    zb_ret_t ret = RET_NOT_IMPLEMENTED;

    if (attr_p->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
    {
        if (attr_p->attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        {
            // ON - OFF management
            uint8_t value = attr_p->values.data8;
            setState((zb_bool_t)value);
        }
    }
    else if (attr_p->cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
    {
        if (attr_p->attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
        {
            // Level management
            uint16_t value = attr_p->values.data16;
            setLevel(value);
        }
    }
    else
    {
        /* Other clusters can be processed here */
        NRF_LOG_INFO("Unhandled cluster attribute id: %d", attr_p->cluster_id);
    }
}

void DimmableLightCTX::setLevelControl(zb_zcl_level_control_set_value_param_t level_p)
{
    uint8_t value = level_p.new_value;
    setLevel(value);
}

// zb_ret_t zb_light_set_attribute(uint8_t ep_id,
//                                 zb_zcl_set_attr_value_param_t * p_savp)
// {
//     bulb_device_ctx_t* p_light_ctx = get_light_ctx_by_id(ep_id);

//     zb_ret_t ret = RET_NOT_IMPLEMENTED;

//     if (p_savp->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
//     {
//         if (p_savp->attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
//         {
//             // ON - OFF management
//             uint8_t value = p_savp->values.data8;
//             zb_light_set_state(ep_id, (zb_bool_t)value);

//             ret = RET_OK;
//         }
//     }
//     else if (p_savp->cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
//     {
//         if (p_savp->attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
//         {
//             // Level management
//             uint16_t value = p_savp->values.data16;
//             zb_light_set_level(ep_id, value);

//             ret = RET_OK;
//         }
//     }
//     else
//     {
//         /* Other clusters can be processed here */
//         NRF_LOG_INFO("Unhandled cluster attribute id: %d", p_savp->cluster_id);
//     }

//     return ret;
// }

/** Sets light at given level and save it in the attributes */
// zb_ret_t zb_light_set_level(uint8_t ep_id,
//                             zb_uint8_t value)
// {
//     bulb_device_ctx_t* p_light_ctx = get_light_ctx_by_id(ep_id);

//     ret_code_t err_code = NRF_SUCCESS;
//     ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
//                          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
//                          ZB_ZCL_CLUSTER_SERVER_ROLE,
//                          ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
//                          (zb_uint8_t *)&value,
//                          ZB_FALSE);

//     if (value == 0) {
//         zb_uint8_t value_onoff = ZB_FALSE;
//         ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
//                              ZB_ZCL_CLUSTER_ID_ON_OFF,
//                              ZB_ZCL_CLUSTER_SERVER_ROLE,
//                              ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
//                              &value_onoff,
//                              ZB_FALSE);
//     }
//     else {
//         zb_uint8_t value_onoff = ZB_TRUE;
//         ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
//                              ZB_ZCL_CLUSTER_ID_ON_OFF,
//                              ZB_ZCL_CLUSTER_SERVER_ROLE,
//                              ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
//                              &value_onoff,
//                              ZB_FALSE);
//     }

//     zb_set_brightness(value, p_light_ctx->ep_id);

//     return (err_code == NRF_SUCCESS ? RET_OK : RET_ERROR);
// }

void DimmableLightCTX::setLevel(zb_uint8_t value)
{
    ZB_ZCL_SET_ATTRIBUTE(ep_id,
                         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
                         (zb_uint8_t *)&value,
                         ZB_FALSE);

    if (value == 0)
    {
        zb_uint8_t value_onoff = ZB_FALSE;
        ZB_ZCL_SET_ATTRIBUTE(ep_id,
                             ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ZB_ZCL_CLUSTER_SERVER_ROLE,
                             ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                             &value_onoff,
                             ZB_FALSE);
    }
    else
    {
        zb_uint8_t value_onoff = ZB_TRUE;
        ZB_ZCL_SET_ATTRIBUTE(ep_id,
                             ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ZB_ZCL_CLUSTER_SERVER_ROLE,
                             ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                             &value_onoff,
                             ZB_FALSE);
    }

    lightCB(value);
}
void DimmableLightCTX::setState(zb_bool_t value)
{
    ZB_ZCL_SET_ATTRIBUTE(ep_id,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&value,
                         ZB_FALSE);

    if (value)
    {
        setLevel(level_control_attr.current_level);
    }
    else
    {
        lightCB(0);
    }
}

// /** Sets light at given state and save it in the attribute */
// void zb_light_set_state(uint8_t ep_id, zb_bool_t value) {
//     bulb_device_ctx_t* p_light_ctx = get_light_ctx_by_id(ep_id);

//     ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
//                          ZB_ZCL_CLUSTER_ID_ON_OFF,
//                          ZB_ZCL_CLUSTER_SERVER_ROLE,
//                          ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
//                          (zb_uint8_t *)&value,
//                          ZB_FALSE);

//     if (value) {
//         zb_light_set_level(p_light_ctx->ep_id, p_light_ctx->level_control_attr.current_level);
//     }
//     else {
//         zb_set_brightness(0, p_light_ctx->ep_id);
//     }
// }
