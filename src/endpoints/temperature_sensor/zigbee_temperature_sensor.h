#ifndef ZIGBEE_TEMP_SENS_H__
#define ZIGBEE_TEMP_SENS_H__

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "../endpoint_enum.h"
#include "../endpoint_ctx.h"


class TemperatureSensorCTX : public EndpointCTX
{
public:
    TemperatureSensorCTX(float period_p, float (*aTempCB)());

    zb_zcl_temp_measurement_attrs_t temp_attr;

    float (*tempCB)();                        // Callback defined in Arduino sketch
    void periodic_CB();                       // Periodic callback
    zb_uint8_t endpoint_CB(zb_bufid_t bufid); // Endpoint specific callback
};

#define TemperatureSensor(CB)                                                                                  \
    TemperatureSensorCTX temp_sens_##CB(30000, CB);                                                            \
    ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list_##CB, &temp_sens_##CB.identify_attr.identify_time); \
    ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list_##CB,                                                 \
                                         &temp_sens_##CB.basic_attr.zcl_version,                               \
                                         &temp_sens_##CB.basic_attr.app_version,                               \
                                         &temp_sens_##CB.basic_attr.stack_version,                             \
                                         &temp_sens_##CB.basic_attr.hw_version,                                \
                                         temp_sens_##CB.basic_attr.mf_name,                                    \
                                         temp_sens_##CB.basic_attr.model_id,                                   \
                                         temp_sens_##CB.basic_attr.date_code,                                  \
                                         &temp_sens_##CB.basic_attr.power_source,                              \
                                         temp_sens_##CB.basic_attr.location_id,                                \
                                         &temp_sens_##CB.basic_attr.ph_env,                                    \
                                         temp_sens_##CB.basic_attr.sw_ver);                                    \
    ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(temperature_attr_list_##CB,                                    \
                                                &temp_sens_##CB.temp_attr.measure_value,                       \                      
                                                &temp_sens_##CB.temp_attr.min_measure_value,                   \
                                                &temp_sens_##CB.temp_attr.max_measure_value,                   \
                                                &temp_sens_##CB.temp_attr.tolerance);                          \
    ZB_HA_DECLARE_TEMPERATURE_SENSOR_CLUSTER_LIST(temperature_sensor_clusters_##CB,                            \
                                                  basic_attr_list_##CB,                                        \
                                                  identify_attr_list_##CB,                                     \
                                                  temperature_attr_list_##CB);                                 \
    ZB_HA_DECLARE_TEMPERATURE_SENSOR_EP_VA(temperature_sensor_ep_##CB,                                         \
                                           temp_sens_##CB.ep_id,                                               \
                                           temperature_sensor_clusters_##CB);                                  \
    bool ep_desc##CB = temp_sens_##CB.set_desc(&temperature_sensor_ep_##CB);                                   \
    int test_i##CB = Zigbee::getInstance().addEP(&temp_sens_##CB);

#define ZB_HA_DECLARE_TEMPERATURE_SENSOR_EP_VA(ep_name, ep_id, cluster_list)        \
    ZB_ZCL_DECLARE_TEMPERATURE_SENSOR_SIMPLE_DESC_VA(                               \
        ep_name,                                                                    \
        ep_id,                                                                      \
        ZB_HA_TEMPERATURE_SENSOR_IN_CLUSTER_NUM,                                    \
        ZB_HA_TEMPERATURE_SENSOR_OUT_CLUSTER_NUM);                                  \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##ep_name,                     \
                                       ZB_HA_TEMPERATURE_SENSOR_REPORT_ATTR_COUNT); \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                                    \
        ep_name,                                                                    \
        ep_id,                                                                      \
        ZB_AF_HA_PROFILE_ID,                                                        \
        0,                                                                          \
        NULL,                                                                       \
        ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
        cluster_list,                                                               \
        (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                          \
        ZB_HA_TEMPERATURE_SENSOR_REPORT_ATTR_COUNT, reporting_info##ep_name, 0, NULL)

#define ZB_ZCL_DECLARE_TEMPERATURE_SENSOR_SIMPLE_DESC_VA(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                                  \
    ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name)                                   \
    simple_desc_##ep_name =                                                                           \
        {                                                                                             \
            ep_id,                                                                                    \
            ZB_AF_HA_PROFILE_ID,                                                                      \
            ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID,                                                       \
            ZB_HA_DEVICE_VER_TEMPERATURE_SENSOR,                                                      \
            0,                                                                                        \
            in_clust_num,                                                                             \
            out_clust_num,                                                                            \
            {                                                                                         \
                ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                           \
                ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                                                   \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                           \
            }}

#endif //ZIGBEE_TEMP_SENS_H__
