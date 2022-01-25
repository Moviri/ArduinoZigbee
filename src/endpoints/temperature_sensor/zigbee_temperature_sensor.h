#ifndef ZIGBEE_TEMP_SENS_H__
#define ZIGBEE_TEMP_SENS_H__

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

typedef struct
{
    uint8_t ep_id;                                      // Endpoint ID
    float (*tempCB)();                                  // Arduino callback
    float period;                                       // Seconds between two measurements
    zb_zcl_basic_attrs_ext_t            basic_attr;
    zb_zcl_identify_attrs_t             identify_attr;
    zb_zcl_temp_measurement_attrs_t     temp_attr;
} temp_sensor_ctx_t;

temp_sensor_ctx_t* get_temp_ctx_by_id(uint8_t ep_id);

bool initialize_temp_sensor_EP(temp_sensor_ctx_t* p_temp_ctx, uint8_t ep_id, float (*aTempCB)(), float period);

void temp_clusters_attr_init(temp_sensor_ctx_t* p_temp_ctx);
void temp_sensor_periodic_CB(uint32_t curr_time);

#define TemperatureSensor(ep_name, ep_id, CB, period)                                                                               \
static temp_sensor_ctx_t m_dev_ctx_## ep_name;                                                                                      \
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list_## ep_name, &m_dev_ctx_## ep_name.identify_attr.identify_time);              \
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list_## ep_name,                                                                    \
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
ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(temperature_attr_list_## ep_name,                                                       \
                                            &m_dev_ctx_## ep_name.temp_attr.measure_value,                                          \                       
                                            &m_dev_ctx_## ep_name.temp_attr.min_measure_value,                                      \
                                            &m_dev_ctx_## ep_name.temp_attr.max_measure_value,                                      \
                                            &m_dev_ctx_## ep_name.temp_attr.tolerance);                                             \
ZB_HA_DECLARE_TEMPERATURE_SENSOR_CLUSTER_LIST(temperature_sensor_clusters_## ep_name,                                               \
                                              basic_attr_list_## ep_name,                                                           \
                                              identify_attr_list_## ep_name,                                                        \
                                              temperature_attr_list_## ep_name);                                                    \
ZB_HA_DECLARE_TEMPERATURE_SENSOR_EP_VA(temperature_sensor_ep_## ep_name,                                                            \
                                    ep_id,                                                                                          \
                                    temperature_sensor_clusters_## ep_name);                                                        \
ep_type_enum type## ep_name = ha_temp_sensor;                                                                                       \
bool init_ep## ep_name = initialize_temp_sensor_EP(&m_dev_ctx_## ep_name, ep_id, CB, period);                                       \
bool ph_add_ep## ep_name = add_EP(&temperature_sensor_ep_## ep_name, type## ep_name);

#define ZB_HA_DECLARE_TEMPERATURE_SENSOR_EP_VA(ep_name, ep_id, cluster_list)                                                        \
    ZB_ZCL_DECLARE_TEMPERATURE_SENSOR_SIMPLE_DESC_VA(                                                                               \
        ep_name,                                                                                                                    \
        ep_id,                                                                                                                      \
        ZB_HA_TEMPERATURE_SENSOR_IN_CLUSTER_NUM,                                                                                    \
        ZB_HA_TEMPERATURE_SENSOR_OUT_CLUSTER_NUM);                                                                                  \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## ep_name,                                                                    \
                                       ZB_HA_TEMPERATURE_SENSOR_REPORT_ATTR_COUNT);                                                 \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                                                                                    \
        ep_name,                                                                                                                    \
        ep_id,                                                                                                                      \
        ZB_AF_HA_PROFILE_ID,                                                                                                        \
        0,                                                                                                                          \
        NULL,                                                                                                                       \
        ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                                                                     \
        cluster_list,                                                                                                               \
        (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                                                                           \
        ZB_HA_TEMPERATURE_SENSOR_REPORT_ATTR_COUNT, reporting_info## ep_name, 0, NULL)

#define ZB_ZCL_DECLARE_TEMPERATURE_SENSOR_SIMPLE_DESC_VA(ep_name, ep_id, in_clust_num, out_clust_num)                               \
    ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                                                                \
    ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name) simple_desc_##ep_name =                                         \
    {                                                                                                                               \
        ep_id,                                                                                                                      \
        ZB_AF_HA_PROFILE_ID,                                                                                                        \
        ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID,                                                                                         \
        ZB_HA_DEVICE_VER_TEMPERATURE_SENSOR,                                                                                        \
        0,                                                                                                                          \
        in_clust_num,                                                                                                               \
        out_clust_num,                                                                                                              \
        {                                                                                                                           \
        ZB_ZCL_CLUSTER_ID_BASIC,                                                                                                    \
        ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                                                 \
        ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                                                                                         \
        ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                                                 \
        }                                                                                                                           \
    }



// #ifdef __cplusplus
// }
// #endif

#endif //ZIGBEE_TEMP_SENS_H__