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
#include "../../sdksupport/zigbee_device.h"

/* Basic cluster attributes initial values. For more information, see section 3.2.2.2 of the ZCL specification. */
#define SENSOR_INIT_BASIC_APP_VERSION 01                                   /**< Version of the application software (1 byte). */
#define SENSOR_INIT_BASIC_STACK_VERSION 10                                 /**< Version of the implementation of the Zigbee stack (1 byte). */
#define SENSOR_INIT_BASIC_HW_VERSION 11                                    /**< Version of the hardware of the device (1 byte). */
#define SENSOR_INIT_BASIC_MANUF_NAME "Arduino"                             /**< Manufacturer name (32 bytes). */
#define SENSOR_INIT_BASIC_MODEL_ID "Temperature Sensor v1"                 /**< Model number assigned by the manufacturer (32-bytes long string). */
#define SENSOR_INIT_BASIC_DATE_CODE "20211111"                             /**< Date provided by the manufacturer of the device in ISO 8601 format (YYYYMMDD), for the first 8 bytes. The remaining 8 bytes are manufacturer-specific. */
#define SENSOR_INIT_BASIC_POWER_SOURCE ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE /**< Type of power source or sources available for the device. For possible values, see section 3.2.2.2.8 of the ZCL specification. */
#define SENSOR_INIT_BASIC_LOCATION_DESC "Generic"                          /**< Description of the physical location of the device (16 bytes). You can modify it during the commisioning process. */
#define SENSOR_INIT_BASIC_PH_ENV ZB_ZCL_BASIC_ENV_UNSPECIFIED              /**< Description of the type of physical environment. For possible values, see section 3.2.2.2.10 of the ZCL specification. */

class TemperatureSensorCTX : public EndpointCTX
{
public:
    TemperatureSensorCTX(float period_p, float (*aTempCB)()) : EndpointCTX(period_p)
    {
        tempCB = aTempCB;

        /* Basic cluster attributes data */
        basic_attr.app_version = SENSOR_INIT_BASIC_APP_VERSION;
        basic_attr.stack_version = SENSOR_INIT_BASIC_STACK_VERSION;
        basic_attr.hw_version = SENSOR_INIT_BASIC_HW_VERSION;

        ZB_ZCL_SET_STRING_VAL(basic_attr.mf_name,
                              SENSOR_INIT_BASIC_MANUF_NAME,
                              ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_MANUF_NAME));

        ZB_ZCL_SET_STRING_VAL(basic_attr.model_id,
                              SENSOR_INIT_BASIC_MODEL_ID,
                              ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_MODEL_ID));

        ZB_ZCL_SET_STRING_VAL(basic_attr.date_code,
                              SENSOR_INIT_BASIC_DATE_CODE,
                              ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_DATE_CODE));

        basic_attr.power_source = SENSOR_INIT_BASIC_POWER_SOURCE;

        ZB_ZCL_SET_STRING_VAL(basic_attr.location_id,
                              SENSOR_INIT_BASIC_LOCATION_DESC,
                              ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_LOCATION_DESC));

        basic_attr.ph_env = SENSOR_INIT_BASIC_PH_ENV;

        /* Temperature measurement cluster attributes data */
        temp_attr.measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_UNKNOWN;
        temp_attr.min_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_MIN_VALUE;
        temp_attr.max_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_MAX_VALUE;
        temp_attr.tolerance = ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_MAX_VALUE;
    }

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
    int test_i##CB = ZIGBEE.addEP(&temp_sens_##CB);

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
