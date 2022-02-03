#ifndef ZIGBEE_DIM_LIGHT_H__
#define ZIGBEE_DIM_LIGHT_H__

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "../endpoint_ctx.h"


class DimmableLightCTX : public EndpointCTX
{
public:
    DimmableLightCTX(float period_p, void (*aLightCB)(const uint8_t brightness_level));

    zb_zcl_scenes_attrs_t scenes_attr;
    zb_zcl_groups_attrs_t groups_attr;
    zb_zcl_on_off_attrs_ext_t on_off_attr;
    zb_zcl_level_control_attrs_t level_control_attr;

    void (*lightCB)(const uint8_t brightness_level); // Callback defined in Arduino sketch
    void periodic_CB();                              // Periodic callback
    zb_uint8_t endpoint_CB(zb_bufid_t bufid);        // Endpoint specific callback

    void setAttribute(zb_zcl_set_attr_value_param_t *attr_p);
    void setLevelControl(zb_zcl_level_control_set_value_param_t level_p);

private:
    void setLevel(zb_uint8_t value);
    void setState(zb_bool_t value);
};
#define DimmableLight(CB)                                                                                      \                                                                                               
    DimmableLightCTX dim_light_##CB(-1, CB);                                                                   \
    ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list_##CB, &dim_light_##CB.identify_attr.identify_time); \
    ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list_##CB, &dim_light_##CB.groups_attr.name_support);        \
    ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list_##CB,                                                   \
                                      &dim_light_##CB.scenes_attr.scene_count,                                 \
                                      &dim_light_##CB.scenes_attr.current_scene,                               \
                                      &dim_light_##CB.scenes_attr.current_group,                               \
                                      &dim_light_##CB.scenes_attr.scene_valid,                                 \
                                      &dim_light_##CB.scenes_attr.name_support);                               \
    ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list_##CB,                                                 \
                                         &dim_light_##CB.basic_attr.zcl_version,                               \
                                         &dim_light_##CB.basic_attr.app_version,                               \
                                         &dim_light_##CB.basic_attr.stack_version,                             \
                                         &dim_light_##CB.basic_attr.hw_version,                                \
                                         dim_light_##CB.basic_attr.mf_name,                                    \
                                         dim_light_##CB.basic_attr.model_id,                                   \
                                         dim_light_##CB.basic_attr.date_code,                                  \
                                         &dim_light_##CB.basic_attr.power_source,                              \
                                         dim_light_##CB.basic_attr.location_id,                                \
                                         &dim_light_##CB.basic_attr.ph_env,                                    \
                                         dim_light_##CB.basic_attr.sw_ver);                                    \
    ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list_##CB,                                               \
                                          &dim_light_##CB.on_off_attr.on_off,                                  \
                                          &dim_light_##CB.on_off_attr.global_scene_ctrl,                       \
                                          &dim_light_##CB.on_off_attr.on_time,                                 \
                                          &dim_light_##CB.on_off_attr.off_wait_time);                          \
    ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_VA(level_control_attr_list_##CB,                                  \
                                                &dim_light_##CB.level_control_attr.current_level,              \
                                                &dim_light_##CB.level_control_attr.remaining_time,             \
                                                dim_light_##CB);                                               \
    ZB_HA_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(dimmable_light_clusters_##CB,                                    \
                                              basic_attr_list_##CB,                                            \
                                              identify_attr_list_##CB,                                         \
                                              groups_attr_list_##CB,                                           \
                                              scenes_attr_list_##CB,                                           \
                                              on_off_attr_list_##CB,                                           \
                                              level_control_attr_list_##CB);                                   \
    ZB_HA_DECLARE_DIMMABLE_LIGHT_EP_VA(dimmable_light_ep_##CB,                                                 \
                                       dim_light_##CB.ep_id,                                                   \
                                       dimmable_light_clusters_##CB);                                          \
    bool ep_desc##CB = dim_light_##CB.set_desc(&dimmable_light_ep_##CB);                                       \
    int ep_add##CB = Zigbee::getInstance().addEP(&dim_light_##CB);

/**@brief Declares attribute list for Level Control cluster, defined as variadic macro.
 *
 * @param[IN] attr_list          Attribure list name.
 * @param[IN] current_level      Pointer to variable to store the current_level attribute value.
 * @param[IN] remaining_time     Pointer to variable to store the remaining_time attribute value.
 * @param[IN] ...                Optional argument to concatenate to the variable name.
 */
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_VA(attr_list, current_level, remaining_time, ...) \
    zb_zcl_level_control_move_status_t move_status_data_ctx##__VA_ARGS__##_attr_list;              \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                    \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, (current_level))              \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, (remaining_time))            \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID,                                 \
                         (&(move_status_data_ctx##__VA_ARGS__##_attr_list)))                       \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**@brief Declares endpoint, uses simple descriptor variadic macro to have multiple endpoints of the same type.
 *
 * @param[IN] ep_name            Endpoint name.
 * @param[IN] ep_id              Endpoint id.
 * @param[IN] cluster_list       List of cluster attribute lists
 */
#define ZB_HA_DECLARE_DIMMABLE_LIGHT_EP_VA(ep_name, ep_id, cluster_list)          \
    ZB_ZCL_DECLARE_DIMMABLE_LIGHT_SIMPLE_DESC_VA(                                 \
        ep_name,                                                                  \
        ep_id,                                                                    \
        ZB_HA_DIMMABLE_LIGHT_IN_CLUSTER_NUM,                                      \
        ZB_HA_DIMMABLE_LIGHT_OUT_CLUSTER_NUM);                                    \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##ep_name,                   \
                                       (ZB_HA_DIMMABLE_LIGHT_REPORT_ATTR_COUNT)); \
    ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info##ep_name,               \
                                           ZB_HA_DIMMABLE_LIGHT_CVC_ATTR_COUNT);  \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                                  \
        ep_name,                                                                  \
        ep_id,                                                                    \
        ZB_AF_HA_PROFILE_ID,                                                      \
        0,                                                                        \
        NULL,                                                                     \
        ZB_ZCL_ARRAY_SIZE(                                                        \
            cluster_list,                                                         \
            zb_zcl_cluster_desc_t),                                               \
        cluster_list,                                                             \
        (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                        \
        ZB_HA_DIMMABLE_LIGHT_REPORT_ATTR_COUNT,                                   \
        reporting_info##ep_name,                                                  \
        ZB_HA_DIMMABLE_LIGHT_CVC_ATTR_COUNT,                                      \
        cvc_alarm_info##ep_name)

/**@brief Declares endpoint, uses simple descriptor variadic macro to have multiple endpoints of the same type.
 *
 * @param[IN] ep_name            Endpoint name.
 * @param[IN] ep_id              Endpoint id.
 * @param[IN] in_clust_num       number of input clusters
 * @param[IN] out_clust_num      number of output clusters
 */
#define ZB_ZCL_DECLARE_DIMMABLE_LIGHT_SIMPLE_DESC_VA(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                              \
    ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name)                               \
    simple_desc_##ep_name =                                                                       \
        {                                                                                         \
            ep_id,                                                                                \
            ZB_AF_HA_PROFILE_ID,                                                                  \
            ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                       \
            ZB_HA_DEVICE_VER_DIMMABLE_LIGHT,                                                      \
            0,                                                                                    \
            in_clust_num,                                                                         \
            out_clust_num,                                                                        \
            {                                                                                     \
                ZB_ZCL_CLUSTER_ID_BASIC,                                                          \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                       \
                ZB_ZCL_CLUSTER_ID_SCENES,                                                         \
                ZB_ZCL_CLUSTER_ID_GROUPS,                                                         \
                ZB_ZCL_CLUSTER_ID_ON_OFF,                                                         \
                ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                  \
            }}

#endif //ZIGBEE_DIM_LIGHT_H__
