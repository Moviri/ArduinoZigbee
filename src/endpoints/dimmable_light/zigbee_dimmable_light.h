#ifndef ZIGBEE_DIM_LIGHT_H__
#define ZIGBEE_DIM_LIGHT_H__

extern "C" {
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "../endpoint_enum.h"
#include "../../sdksupport/zigbee_device.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

// Zigbee dimmable light device context.
typedef struct
{
  uint8_t ep_id;                                        // Endpoint ID
  void (*bulbCB)(const uint8_t brightness_level);       // Arduino callback
  zb_zcl_basic_attrs_ext_t basic_attr;
  zb_zcl_identify_attrs_t identify_attr;
  zb_zcl_scenes_attrs_t scenes_attr;
  zb_zcl_groups_attrs_t groups_attr;
  zb_zcl_on_off_attrs_ext_t on_off_attr;
  zb_zcl_level_control_attrs_t level_control_attr;
} bulb_device_ctx_t;

/** @brief Return dimmable light context by endpoint id.
 *
 * @param[in] ep_id       Endpoint ID
 */
bulb_device_ctx_t* get_light_ctx_by_id(uint8_t ep_id);

/**
 * @brief Initializes dimmable light context object.
 *
 * @param[in] p_light_ctx A pointer to light context object.
 * @param[in] ep_id       Endpoint ID
 */
bool zb_light_init_ctx(bulb_device_ctx_t * p_light_ctx,
                             uint8_t     ep_id);

void light_clusters_attr_init(bulb_device_ctx_t * p_light_ctx);

/**@brief Sets color light attribute.
 *
 * This function sets physical property of the light according to the provided
 * attribute value.
 *
 * @param[in] p_light_ctx  Pointer to light context object.
 * @param[in] p_savp       Pointer to attribute value.
 *
 * @return RET_OK on success or error code on failure.
 */
zb_ret_t zb_light_set_attribute(uint8_t ep_id,
                                zb_zcl_set_attr_value_param_t * p_savp);

/**@brief Sets color light brightness.
 *
 * @param[in] p_light_ctx  Pointer to light context object.
 * @param[in] value        Brightness level.
 *
 * @return RET_OK on success or error code on failure.
 */
zb_ret_t zb_light_set_level(uint8_t ep_id,
                            zb_uint8_t  value);

/**@brief Sets light state(On/Off).
 *
 * @param[in] ep_id        endpoint id.
 * @param[in] value        state.
 *
 */
void zb_light_set_state(uint8_t ep_id, zb_bool_t value);


zb_uint8_t zcl_light_endpoint_cb(zb_bufid_t bufid);

bool initialize_light_EP(bulb_device_ctx_t* p_light_ctx, uint8_t ep_id, void (*aBulbCB)(const uint8_t brightness_level));

#define DimmableLight(ep_name, ep_id, CB)                                                                                           \                                                                                                           
    static bulb_device_ctx_t m_dev_ctx_## ep_name;                                                                                  \
    ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list_## ep_name, &m_dev_ctx_## ep_name.identify_attr.identify_time);          \
    ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list_## ep_name, &m_dev_ctx_## ep_name.groups_attr.name_support);                 \
    ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list_## ep_name,                                                                  \
                                    &m_dev_ctx_## ep_name.scenes_attr.scene_count,                                                  \
                                    &m_dev_ctx_## ep_name.scenes_attr.current_scene,                                                \
                                    &m_dev_ctx_## ep_name.scenes_attr.current_group,                                                \
                                    &m_dev_ctx_## ep_name.scenes_attr.scene_valid,                                                  \
                                    &m_dev_ctx_## ep_name.scenes_attr.name_support);                                                \
    ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list_## ep_name,                                                                \
                                        &m_dev_ctx_## ep_name.basic_attr.zcl_version,                                               \
                                        &m_dev_ctx_## ep_name.basic_attr.app_version,                                               \
                                        &m_dev_ctx_## ep_name.basic_attr.stack_version,                                             \
                                        &m_dev_ctx_## ep_name.basic_attr.hw_version,                                                \
                                        m_dev_ctx_## ep_name.basic_attr.mf_name,                                                    \
                                        m_dev_ctx_## ep_name.basic_attr.model_id,                                                   \
                                        m_dev_ctx_## ep_name.basic_attr.date_code,                                                  \
                                        &m_dev_ctx_## ep_name.basic_attr.power_source,                                              \
                                        m_dev_ctx_## ep_name.basic_attr.location_id,                                                \
                                        &m_dev_ctx_## ep_name.basic_attr.ph_env,                                                    \
                                        m_dev_ctx_## ep_name.basic_attr.sw_ver);                                                    \
    ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list_## ep_name,                                                              \
                                        &m_dev_ctx_## ep_name.on_off_attr.on_off,                                                   \
                                        &m_dev_ctx_## ep_name.on_off_attr.global_scene_ctrl,                                        \
                                        &m_dev_ctx_## ep_name.on_off_attr.on_time,                                                  \
                                        &m_dev_ctx_## ep_name.on_off_attr.off_wait_time);                                           \
    ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_VA(level_control_attr_list_## ep_name,                                                 \
                                            &m_dev_ctx_## ep_name.level_control_attr.current_level,                                 \
                                            &m_dev_ctx_## ep_name.level_control_attr.remaining_time,                                \
                                            m_dev_ctx_## ep_name);                                                                  \
    ZB_HA_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(dimmable_light_clusters_## ep_name,                                                   \
                                            basic_attr_list_## ep_name,                                                             \
                                            identify_attr_list_## ep_name,                                                          \
                                            groups_attr_list_## ep_name,                                                            \
                                            scenes_attr_list_## ep_name,                                                            \
                                            on_off_attr_list_## ep_name,                                                            \
                                            level_control_attr_list_## ep_name);                                                    \
    ZB_HA_DECLARE_DIMMABLE_LIGHT_EP_VA(dimmable_light_ep_## ep_name,                                                                \
                                    ep_id,                                                                                          \
                                    dimmable_light_clusters_## ep_name);                                                            \
    ep_type_enum type## ep_name = ha_dimmable_light;                                                                                \
    bool init_ep## ep_name = initialize_light_EP(&m_dev_ctx_## ep_name, ep_id, CB);                                                 \
    bool ph_add_ep## ep_name = add_EP(&dimmable_light_ep_## ep_name, type## ep_name);

/**@brief Declares attribute list for Level Control cluster, defined as variadic macro.
 *
 * @param[IN] attr_list          Attribure list name.
 * @param[IN] current_level      Pointer to variable to store the current_level attribute value.
 * @param[IN] remaining_time     Pointer to variable to store the remaining_time attribute value.
 * @param[IN] ...                Optional argument to concatenate to the variable name.
 */
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_VA(attr_list, current_level, remaining_time, ...)                                  \
    zb_zcl_level_control_move_status_t move_status_data_ctx## __VA_ARGS__## _attr_list ;                                            \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, (current_level))                                               \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, (remaining_time))                                             \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID,                                                                  \
                        (&(move_status_data_ctx## __VA_ARGS__## _attr_list)))                                                       \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**@brief Declares endpoint, uses simple descriptor variadic macro to have multiple endpoints of the same type.
 *
 * @param[IN] ep_name            Endpoint name.
 * @param[IN] ep_id              Endpoint id.
 * @param[IN] cluster_list       List of cluster attribute lists
 */
#define ZB_HA_DECLARE_DIMMABLE_LIGHT_EP_VA(ep_name, ep_id, cluster_list)                                                            \
    ZB_ZCL_DECLARE_DIMMABLE_LIGHT_SIMPLE_DESC_VA(                                                                                   \
        ep_name,                                                                                                                    \
        ep_id,                                                                                                                      \
        ZB_HA_DIMMABLE_LIGHT_IN_CLUSTER_NUM,                                                                                        \
        ZB_HA_DIMMABLE_LIGHT_OUT_CLUSTER_NUM);                                                                                      \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## ep_name,                                                                    \
        (ZB_HA_DIMMABLE_LIGHT_REPORT_ATTR_COUNT));                                                                                  \
    ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info## ep_name,                                                                \
        ZB_HA_DIMMABLE_LIGHT_CVC_ATTR_COUNT);                                                                                       \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                                                                                    \
        ep_name,                                                                                                                    \
        ep_id,                                                                                                                      \
        ZB_AF_HA_PROFILE_ID,                                                                                                        \
        0,                                                                                                                          \
        NULL,                                                                                                                       \
        ZB_ZCL_ARRAY_SIZE(                                                                                                          \
            cluster_list,                                                                                                           \
            zb_zcl_cluster_desc_t),                                                                                                 \
        cluster_list,                                                                                                               \
        (zb_af_simple_desc_1_1_t*)&simple_desc_## ep_name,                                                                          \
        ZB_HA_DIMMABLE_LIGHT_REPORT_ATTR_COUNT,                                                                                     \
        reporting_info## ep_name,                                                                                                   \
        ZB_HA_DIMMABLE_LIGHT_CVC_ATTR_COUNT,                                                                                        \
        cvc_alarm_info## ep_name)


/**@brief Declares endpoint, uses simple descriptor variadic macro to have multiple endpoints of the same type.
 *
 * @param[IN] ep_name            Endpoint name.
 * @param[IN] ep_id              Endpoint id.
 * @param[IN] in_clust_num       number of input clusters
 * @param[IN] out_clust_num      number of output clusters
 */
#define ZB_ZCL_DECLARE_DIMMABLE_LIGHT_SIMPLE_DESC_VA(ep_name, ep_id, in_clust_num, out_clust_num)                                   \
    ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                                                                \
    ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name)  simple_desc_## ep_name =                                       \
    {                                                                                                                               \
        ep_id,                                                                                                                      \
        ZB_AF_HA_PROFILE_ID,                                                                                                        \
        ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                                                             \
        ZB_HA_DEVICE_VER_DIMMABLE_LIGHT,                                                                                            \
        0,                                                                                                                          \
        in_clust_num,                                                                                                               \
        out_clust_num,                                                                                                              \
        {                                                                                                                           \
        ZB_ZCL_CLUSTER_ID_BASIC,                                                                                                    \
        ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                                                 \
            ZB_ZCL_CLUSTER_ID_SCENES,                                                                                               \
        ZB_ZCL_CLUSTER_ID_GROUPS,                                                                                                   \
        ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                                                   \
        ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                                                            \
        }                                                                                                                           \
    }  

// #ifdef __cplusplus
// }
// #endif

#endif //ZIGBEE_DIM_LIGHT_H__
