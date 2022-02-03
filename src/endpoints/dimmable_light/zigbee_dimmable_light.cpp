#include <mbed.h>
#include "zigbee_dimmable_light.h"
#include <vector>
#include "../../Zigbee.h"

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

    m_zboss_specific_data.cluster_revision_scenes_attr_list = 1;
    zb_zcl_attr_t scenes_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_specific_data.cluster_revision_scenes_attr_list)},
        {ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID, 0x20U, 0x01U, (void *)(&scenes_attr.scene_count)},
        {ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID, 0x20U, 0x01U, (void *)(&scenes_attr.current_scene)},
        {ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID, 0x21U, 0x01U, (void *)(&scenes_attr.current_group)},
        {ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID, 0x10U, 0x01U, (void *)(&scenes_attr.scene_valid)},
        {ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID, 0x18U, 0x01U, (void *)(&scenes_attr.name_support)},
        {(zb_uint16_t)(-1), 0, 0, __null}};

    static_assert(sizeof(m_zboss_specific_data.scenes_attr_list) == sizeof(scenes_attr_list), "Check vector size");
    memcpy(m_zboss_specific_data.scenes_attr_list, scenes_attr_list, sizeof(scenes_attr_list));

    m_zboss_specific_data.cluster_revision_groups_attr_list = 1;
    zb_zcl_attr_t groups_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_specific_data.cluster_revision_groups_attr_list)},
        {ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID, 0x18U, 0x01U, (void *)(&groups_attr.name_support)},
        {(zb_uint16_t)(-1), 0, 0, __null}};

    static_assert(sizeof(m_zboss_specific_data.groups_attr_list) == sizeof(groups_attr_list), "Check vector size");
    memcpy(m_zboss_specific_data.groups_attr_list, groups_attr_list, sizeof(groups_attr_list));

    m_zboss_specific_data.cluster_revision_on_off_attr_list = 1;
    zb_zcl_attr_t on_off_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_specific_data.cluster_revision_on_off_attr_list)},
        {ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, 0x10U, 0x01U | 0x04U | 0x10U, (void *)(&on_off_attr.on_off)},
        {ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL, 0x10U, 0x01U, (void *)(&on_off_attr.global_scene_ctrl)},
        {ZB_ZCL_ATTR_ON_OFF_ON_TIME, 0x21U, (0x01U | 0x02U), (void *)(&on_off_attr.on_time)},
        {ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME, 0x21U, (0x01U | 0x02U), (void *)(&on_off_attr.off_wait_time)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_specific_data.on_off_attr_list) == sizeof(on_off_attr_list), "Check vector size");
    memcpy(m_zboss_specific_data.on_off_attr_list, on_off_attr_list, sizeof(on_off_attr_list));

    m_zboss_specific_data.cluster_revision_level_control_attr_list = 1;
    zb_zcl_attr_t level_control_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_specific_data.cluster_revision_level_control_attr_list)},
        {ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 0x20U, 0x01U | 0x04U | 0x10U, (void *)(&level_control_attr.current_level)},
        {ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, 0x21U, 0x01U, (void *)(&level_control_attr.remaining_time)},
        {ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID, 0x00U, 0x40U, (void *)(&(m_zboss_specific_data.move_status_data_ctxdim_light))},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_specific_data.level_control_attr_list) == sizeof(level_control_attr_list), "Check vector size");
    memcpy(m_zboss_specific_data.level_control_attr_list, level_control_attr_list, sizeof(level_control_attr_list));

    zb_zcl_cluster_desc_t dimmable_light_clusters[] = {
        {(0x0003U), ((sizeof(m_zboss_data.identify_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.identify_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_identify_init_server : (((0x01U) == 0x02U) ? zb_zcl_identify_init_client : __null))},
        {(0x0000U), ((sizeof(m_zboss_data.basic_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.basic_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_basic_init_server : (((0x01U) == 0x02U) ? zb_zcl_basic_init_client : __null))},
        {(0x0005U), ((sizeof(m_zboss_specific_data.scenes_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_specific_data.scenes_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_scenes_init_server : (((0x01U) == 0x02U) ? zb_zcl_scenes_init_client : __null))},
        {(0x0004U), ((sizeof(m_zboss_specific_data.groups_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_specific_data.groups_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_groups_init_server : (((0x01U) == 0x02U) ? zb_zcl_groups_init_client : __null))},
        {(0x0006U), ((sizeof(m_zboss_specific_data.on_off_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_specific_data.on_off_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_on_off_init_server : (((0x01U) == 0x02U) ? zb_zcl_on_off_init_client : __null))},
        {(0x0008U), ((sizeof(m_zboss_specific_data.level_control_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_specific_data.level_control_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_level_control_init_server : (((0x01U) == 0x02U) ? zb_zcl_level_control_init_client : __null))}};
    static_assert(sizeof(m_zboss_specific_data.dimmable_light_clusters) == sizeof(dimmable_light_clusters), "Check vector size");
    memcpy(m_zboss_specific_data.dimmable_light_clusters, dimmable_light_clusters, sizeof(dimmable_light_clusters));

    m_zboss_specific_data.simple_desc = {
        ep_id,
        0x0104U,
        ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,
        1,
        0,
        6,
        0,
        {0x0000U, 0x0003U, 0x0005U, 0x0004U, 0x0006U, 0x0008U}};

    m_zboss_specific_data.dimmable_light_ep = {
        ep_id,
        0x0104U,
        __null,
        __null,
        0,
        (void *)__null,
        (sizeof(m_zboss_specific_data.dimmable_light_clusters) / sizeof(zb_zcl_cluster_desc_t)),
        m_zboss_specific_data.dimmable_light_clusters,
        (zb_af_simple_desc_1_1_t *)&m_zboss_specific_data.simple_desc,
        (1 + 1),
        m_zboss_specific_data.reporting_infodimmable_light,
        1,
        m_zboss_specific_data.cvc_alarm_infodimmable_light};

    set_desc(&m_zboss_specific_data.dimmable_light_ep);
    Zigbee::getInstance().addEP(this);
}

zb_uint8_t DimmableLightCTX::endpoint_CB(zb_bufid_t bufid)
{
    zb_bufid_t zcl_cmd_buf = bufid;
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);

    if (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV &&
        !cmd_info->is_common_command &&
        cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
        cmd_info->cmd_id == ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID)
    {
        setState((zb_bool_t) false);

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
