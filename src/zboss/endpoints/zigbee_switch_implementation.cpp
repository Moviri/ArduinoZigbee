#include <mbed.h>
#include "zigbee_switch_implementation.h"
#include "../../endpoints/zigbee_switch.h"

#define interface() static_cast<ZigbeeSwitch *>(m_interface)

ZigbeeSwitchImplementation::ZigbeeSwitchImplementation(ZigbeeSwitch *interface, const zb_char_t model_id[], unsigned int power_source_type) : ZigbeeSwitchImplementation(interface, model_id, power_source_type)
{
    /* WARNING: do not use the interface object inside this constructor because it is not fully constructed. */
    memset(&m_zboss_data, 0, sizeof(m_zboss_data));

    zb_zcl_cluster_desc_t switch_clusters[] = {{(ZB_ZCL_CLUSTER_ID_IDENTIFY),
                                                ((sizeof(identify_attr_list) / sizeof(zb_zcl_attr_t))),
                                                ((identify_attr_list)),
                                                (ZB_ZCL_CLUSTER_SERVER_ROLE),
                                                (0x0000),
                                                (((ZB_ZCL_CLUSTER_SERVER_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_identify_init_server : (((ZB_ZCL_CLUSTER_SERVER_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_identify_init_client : 0))},

                                               {(ZB_ZCL_CLUSTER_ID_BASIC),
                                                ((sizeof(basic_attr_list) / sizeof(zb_zcl_attr_t))),
                                                ((basic_attr_list)),
                                                (ZB_ZCL_CLUSTER_SERVER_ROLE),
                                                (0x0000),
                                                (((ZB_ZCL_CLUSTER_SERVER_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_basic_init_server : (((ZB_ZCL_CLUSTER_SERVER_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_basic_init_client : 0))},

                                               {(ZB_ZCL_CLUSTER_ID_IDENTIFY),
                                                (0),
                                                (0),
                                                (ZB_ZCL_CLUSTER_CLIENT_ROLE),
                                                (0x0000),
                                                (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_identify_init_server : (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_identify_init_client : 0))},

                                               {(ZB_ZCL_CLUSTER_ID_SCENES),
                                                (0),
                                                (0),
                                                (ZB_ZCL_CLUSTER_CLIENT_ROLE),
                                                (0x0000),
                                                (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_scenes_init_server : (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_scenes_init_client : 0))},

                                               {(ZB_ZCL_CLUSTER_ID_GROUPS),
                                                (0),
                                                (0),
                                                (ZB_ZCL_CLUSTER_CLIENT_ROLE),
                                                (0x0000),
                                                (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_groups_init_server : (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_groups_init_client : 0))},

                                               {(ZB_ZCL_CLUSTER_ID_ON_OFF),
                                                (0),
                                                (0),
                                                (ZB_ZCL_CLUSTER_CLIENT_ROLE),
                                                (0x0000),
                                                (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_on_off_init_server : (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_on_off_init_client : 0))},

                                               {(ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL),
                                                (0),
                                                (0),
                                                (ZB_ZCL_CLUSTER_CLIENT_ROLE), (0x0000), (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? zb_zcl_level_control_init_server : (((ZB_ZCL_CLUSTER_CLIENT_ROLE) == ZB_ZCL_CLUSTER_CLIENT_ROLE) ? zb_zcl_level_control_init_client : 0))}};
    static_assert(sizeof(m_zboss_data.switch_clusters) == sizeof(switch_clusters), "Check vector size");
    memcpy(m_zboss_data.switch_clusters, switch_clusters, sizeof(switch_clusters));

    m_zboss_data.simple_desc = {
        m_endpoint_id,
        ZB_AF_HA_PROFILE_ID,
        ZB_HA_DIMMER_SWITCH_DEVICE_ID,
        0,
        0,
        2,
        5,
        {ZB_ZCL_CLUSTER_ID_BASIC,
         ZB_ZCL_CLUSTER_ID_IDENTIFY,
         ZB_ZCL_CLUSTER_ID_ON_OFF,
         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
         ZB_ZCL_CLUSTER_ID_SCENES,
         ZB_ZCL_CLUSTER_ID_GROUPS,
         ZB_ZCL_CLUSTER_ID_IDENTIFY}};

    m_zboss_data.switch_ep = {
        m_endpoint_id,
        ZB_AF_HA_PROFILE_ID,
        0,
        0,
        0,
        (zb_voidp_t)0,
        (sizeof(dimmer_switch_clusters) / sizeof(zb_zcl_cluster_desc_t)),
        dimmer_switch_clusters,
        (zb_af_simple_desc_1_1_t *)&simple_desc_dimmer_switch_ep,
        0,
        0,
        0,
        0};

    setDescriptor(&m_zboss_data.switch_ep);
}

zb_ret_t ZigbeeSwitchImplementation::processCommandDV(zb_zcl_device_callback_param_t *cmd_params)
{
    switch (cmd_params->device_cb_id)
    {
    case ZB_ZCL_IDENTIFY_EFFECT_CB_ID:
    {
        onIdentify(&cmd_params->cb_param.identify_effect_value_param);
        return RET_OK;
    }
    default:
        return RET_ERROR;
    }
}

void ZigbeeSwitchImplementation::update()
{
    uint8_t val_p = interface()->m_read_switch_p();
    bool val_b = interface()->m_read_switch_b();

    if (prev_val_b != val_b) {
        prev_val_b = val_b;
        sendOnOffMsg(val_b);
    }
}

int ZigbeeSwitchImplementation::sendLevelMsg(int brightness)
{

}

bool ZigbeeSwitchImplementation::sendOnOffMsg(bool state)
{

}
