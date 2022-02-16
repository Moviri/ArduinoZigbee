#include <mbed.h>
#include "zigbee_dimmable_light_implementation.h"
#include "../../endpoints/zigbee_dimmable_light.h"
#include <vector>

ZigbeeDimmableLightImplementation::ZigbeeDimmableLightImplementation(ZigbeeDimmableLight *interface, zb_char_t model_id[], unsigned int power_source_type) : ZigbeeEndpointImplementation(model_id, power_source_type), m_interface(interface)
{
    /* WARNING: do not use the interface object inside this constructor because it is not fully constructed. */
    memset(&m_zboss_data, 0, sizeof(m_zboss_data));

    /* On/Off cluster attributes data */
    m_zboss_data.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;

    /* Level control cluster attributes data */
    m_zboss_data.level_control_attr.current_level = ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE; // Set current level value to maximum
    m_zboss_data.level_control_attr.remaining_time = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;

    m_zboss_data.cluster_revision_scenes_attr_list = 1;
    zb_zcl_attr_t scenes_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_scenes_attr_list)},
        {ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID, 0x20U, 0x01U, (void *)(&m_zboss_data.scenes_attr.scene_count)},
        {ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID, 0x20U, 0x01U, (void *)(&m_zboss_data.scenes_attr.current_scene)},
        {ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID, 0x21U, 0x01U, (void *)(&m_zboss_data.scenes_attr.current_group)},
        {ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID, 0x10U, 0x01U, (void *)(&m_zboss_data.scenes_attr.scene_valid)},
        {ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID, 0x18U, 0x01U, (void *)(&m_zboss_data.scenes_attr.name_support)},
        {(zb_uint16_t)(-1), 0, 0, __null}};

    static_assert(sizeof(m_zboss_data.scenes_attr_list) == sizeof(scenes_attr_list), "Check vector size");
    memcpy(m_zboss_data.scenes_attr_list, scenes_attr_list, sizeof(scenes_attr_list));

    m_zboss_data.cluster_revision_groups_attr_list = 1;
    zb_zcl_attr_t groups_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_groups_attr_list)},
        {ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID, 0x18U, 0x01U, (void *)(&m_zboss_data.groups_attr.name_support)},
        {(zb_uint16_t)(-1), 0, 0, __null}};

    static_assert(sizeof(m_zboss_data.groups_attr_list) == sizeof(groups_attr_list), "Check vector size");
    memcpy(m_zboss_data.groups_attr_list, groups_attr_list, sizeof(groups_attr_list));

    m_zboss_data.cluster_revision_on_off_attr_list = 1;
    zb_zcl_attr_t on_off_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_on_off_attr_list)},
        {ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, 0x10U, 0x01U | 0x04U | 0x10U, (void *)(&m_zboss_data.on_off_attr.on_off)},
        {ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL, 0x10U, 0x01U, (void *)(&m_zboss_data.on_off_attr.global_scene_ctrl)},
        {ZB_ZCL_ATTR_ON_OFF_ON_TIME, 0x21U, (0x01U | 0x02U), (void *)(&m_zboss_data.on_off_attr.on_time)},
        {ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME, 0x21U, (0x01U | 0x02U), (void *)(&m_zboss_data.on_off_attr.off_wait_time)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_data.on_off_attr_list) == sizeof(on_off_attr_list), "Check vector size");
    memcpy(m_zboss_data.on_off_attr_list, on_off_attr_list, sizeof(on_off_attr_list));

    m_zboss_data.cluster_revision_level_control_attr_list = 1;
    zb_zcl_attr_t level_control_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_level_control_attr_list)},
        {ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 0x20U, 0x01U | 0x04U | 0x10U, (void *)(&m_zboss_data.level_control_attr.current_level)},
        {ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, 0x21U, 0x01U, (void *)(&m_zboss_data.level_control_attr.remaining_time)},
        {ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID, 0x00U, 0x40U, (void *)(&(m_zboss_data.move_status_data_ctxdim_light))},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_data.level_control_attr_list) == sizeof(level_control_attr_list), "Check vector size");
    memcpy(m_zboss_data.level_control_attr_list, level_control_attr_list, sizeof(level_control_attr_list));

    zb_zcl_cluster_desc_t dimmable_light_clusters[] = {
        {(0x0003U), ((sizeof(m_zboss_basic_data.identify_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_basic_data.identify_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_identify_init_server : (((0x01U) == 0x02U) ? zb_zcl_identify_init_client : __null))},
        {(0x0000U), ((sizeof(m_zboss_basic_data.basic_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_basic_data.basic_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_basic_init_server : (((0x01U) == 0x02U) ? zb_zcl_basic_init_client : __null))},
        {(0x0005U), ((sizeof(m_zboss_data.scenes_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.scenes_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_scenes_init_server : (((0x01U) == 0x02U) ? zb_zcl_scenes_init_client : __null))},
        {(0x0004U), ((sizeof(m_zboss_data.groups_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.groups_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_groups_init_server : (((0x01U) == 0x02U) ? zb_zcl_groups_init_client : __null))},
        {(0x0006U), ((sizeof(m_zboss_data.on_off_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.on_off_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_on_off_init_server : (((0x01U) == 0x02U) ? zb_zcl_on_off_init_client : __null))},
        {(0x0008U), ((sizeof(m_zboss_data.level_control_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.level_control_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_level_control_init_server : (((0x01U) == 0x02U) ? zb_zcl_level_control_init_client : __null))}};
    static_assert(sizeof(m_zboss_data.dimmable_light_clusters) == sizeof(dimmable_light_clusters), "Check vector size");
    memcpy(m_zboss_data.dimmable_light_clusters, dimmable_light_clusters, sizeof(dimmable_light_clusters));

    m_zboss_data.simple_desc = {
        m_endpoint_id,
        0x0104U,
        ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,
        1,
        0,
        6,
        0,
        {0x0000U, 0x0003U, 0x0005U, 0x0004U, 0x0006U, 0x0008U}};

    m_zboss_data.dimmable_light_ep = {
        m_endpoint_id,
        0x0104U,
        __null,
        __null,
        0,
        (void *)__null,
        (sizeof(m_zboss_data.dimmable_light_clusters) / sizeof(zb_zcl_cluster_desc_t)),
        m_zboss_data.dimmable_light_clusters,
        (zb_af_simple_desc_1_1_t *)&m_zboss_data.simple_desc,
        (1 + 1),
        m_zboss_data.reporting_infodimmable_light,
        1,
        m_zboss_data.cvc_alarm_infodimmable_light};

    setDescriptor(&m_zboss_data.dimmable_light_ep);
}

zb_uint8_t ZigbeeDimmableLightImplementation::processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params)
{
    if (cmd_params->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV &&
        !cmd_params->is_common_command &&
        cmd_params->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
        cmd_params->cmd_id == ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID)
    {
        setState((zb_bool_t) false);

        return ZB_TRUE;
    }
    return ZB_FALSE;
}

zb_ret_t ZigbeeDimmableLightImplementation::processCommandDV(zb_zcl_device_callback_param_t *cmd_params)
{
    switch (cmd_params->device_cb_id)
    {
    case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID:
        // Inform user about ZCL Level Control cluster attributes value modification
        setBrightness(cmd_params->cb_param.level_control_set_value_param.new_value);
        return RET_OK;
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
        // Inform user about attribute value modification
        {
            zb_uint8_t result = setAttribute(&cmd_params->cb_param.set_attr_value_param);
            return (result == ZB_TRUE) ? RET_OK : RET_ERROR;
        }
    case ZB_ZCL_IDENTIFY_EFFECT_CB_ID:
    {
        feedbackEffect(&cmd_params->cb_param.identify_effect_value_param);
        return RET_OK;
    }
    default:
        return RET_ERROR;
    }
}

zb_uint8_t ZigbeeDimmableLightImplementation::setAttribute(zb_zcl_set_attr_value_param_t *attr_p)
{
    switch (attr_p->cluster_id)
    {
    case ZB_ZCL_CLUSTER_ID_ON_OFF:
        if (attr_p->attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        {
            // ON - OFF management
            uint8_t value = attr_p->values.data8;
            setState((zb_bool_t)value);
            return ZB_TRUE;
        }
        break;
    case ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL:
        if (attr_p->attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
        {
            // Level management
            uint16_t value = attr_p->values.data16;
            setBrightness(value);
            return ZB_TRUE;
        }
        break;
    default:;
    }
    /* Other clusters can be processed here */
    NRF_LOG_INFO("Unhandled cluster attribute id: %d", attr_p->cluster_id);
    return ZB_FALSE;
}

void ZigbeeDimmableLightImplementation::setBrightness(zb_uint8_t value)
{
    ZB_ZCL_SET_ATTRIBUTE(m_endpoint_id,
                         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
                         (zb_uint8_t *)&value,
                         ZB_FALSE);

    zb_uint8_t value_onoff = (value == 0) ? ZB_FALSE : ZB_TRUE;
    ZB_ZCL_SET_ATTRIBUTE(m_endpoint_id,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         &value_onoff,
                         ZB_FALSE);

    m_interface->m_write_brightness(value);
}

void ZigbeeDimmableLightImplementation::setState(zb_bool_t value)
{
    ZB_ZCL_SET_ATTRIBUTE(m_endpoint_id,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&value,
                         ZB_FALSE);

    if (value)
    {
        setBrightness(m_zboss_data.level_control_attr.current_level);
    }
    else
    {
        m_interface->m_write_brightness(0);
    }
}
