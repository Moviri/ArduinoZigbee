#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include <vector>
#include "../../Zigbee.h"

TemperatureSensorCTX::TemperatureSensorCTX(float period_p, float (*aTempCB)()) : EndpointCTX(period_p)
{
    tempCB = aTempCB;
    // tempCB(); // initialize temperature value to avoid bad read from other devices

    /* Basic cluster attributes data */
    basic_attr.app_version = 01;   /**< Version of the application software (1 byte). */
    basic_attr.stack_version = 10; /**< Version of the implementation of the Zigbee stack (1 byte). */
    basic_attr.hw_version = 11;    /**< Version of the hardware of the device (1 byte). */

    ZB_ZCL_SET_STRING_VAL(basic_attr.mf_name, /**< Manufacturer name (32 bytes). */
                          "Arduino",
                          ZB_ZCL_STRING_CONST_SIZE("Arduino"));

    ZB_ZCL_SET_STRING_VAL(basic_attr.model_id, /**< Model number assigned by the manufacturer (32-bytes long string). */
                          "Temperature Sensor v1",
                          ZB_ZCL_STRING_CONST_SIZE("Temperature Sensor v1"));

    ZB_ZCL_SET_STRING_VAL(basic_attr.date_code, /**< Date provided by the manufacturer of the device in ISO 8601 format (YYYYMMDD), for the first 8 bytes. The remaining 8 bytes are manufacturer-specific. */
                          "20211111",
                          ZB_ZCL_STRING_CONST_SIZE("20211111"));

    basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE; /**< Type of power source or sources available for the device. For possible values, see section 3.2.2.2.8 of the ZCL specification. */

    ZB_ZCL_SET_STRING_VAL(basic_attr.location_id, /**< Description of the physical location of the device (16 bytes). You can modify it during the commisioning process. */
                          "Generic",
                          ZB_ZCL_STRING_CONST_SIZE("Generic"));

    basic_attr.ph_env = ZB_ZCL_BASIC_ENV_UNSPECIFIED; /**< Description of the type of physical environment. For possible values, see section 3.2.2.2.10 of the ZCL specification. */

    /* Temperature measurement cluster attributes data */
    temp_attr.measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_UNKNOWN;
    temp_attr.min_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_MIN_VALUE;
    temp_attr.max_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_MAX_VALUE;
    temp_attr.tolerance = ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_MAX_VALUE;

    m_zboss_specific_data.cluster_revision_temperature_attr_list = 1;
    zb_zcl_attr_t temperature_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_specific_data.cluster_revision_temperature_attr_list)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, 0x29U, 0x01U | 0x04U, (void *)(&temp_attr.measure_value)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, 0x29U, 0x01U, (void *)(&temp_attr.min_measure_value)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, 0x29U, 0x01U, (void *)(&temp_attr.max_measure_value)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID, 0x21U, 0x01U, (void *)(&temp_attr.tolerance)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_specific_data.temperature_attr_list) == sizeof(temperature_attr_list), "Check vector size");
    memcpy(m_zboss_specific_data.temperature_attr_list, temperature_attr_list, sizeof(temperature_attr_list));

    zb_zcl_cluster_desc_t temperature_sensor_clusters[] = {
        {(0x0003U), ((sizeof(m_zboss_data.identify_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.identify_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_identify_init_server : (((0x01U) == 0x02U) ? zb_zcl_identify_init_client : __null))},
        {(0x0000U), ((sizeof(m_zboss_data.basic_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.basic_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_basic_init_server : (((0x01U) == 0x02U) ? zb_zcl_basic_init_client : __null))},
        {(0x0402U), ((sizeof(m_zboss_specific_data.temperature_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_specific_data.temperature_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_temp_measurement_init_server : (((0x01U) == 0x02U) ? zb_zcl_temp_measurement_init_client : __null))},
        {(0x0003U), (0), (__null), (0x02U), (0x0000), (((0x02U) == 0x01U) ? zb_zcl_identify_init_server : (((0x02U) == 0x02U) ? zb_zcl_identify_init_client : __null))}};
    static_assert(sizeof(m_zboss_specific_data.temperature_sensor_clusters) == sizeof(temperature_sensor_clusters), "Check vector size");
    memcpy(m_zboss_specific_data.temperature_sensor_clusters, temperature_sensor_clusters, sizeof(temperature_sensor_clusters));

    m_zboss_specific_data.simple_desc_temperature_sensor = {
        ep_id,
        0x0104U,
        ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID,
        0,
        0,
        3,
        1,
        {0x0000U, 0x0003U, 0x0402U, 0x0003U}};
    m_zboss_specific_data.temperature_sensor = {
        ep_id,
        0x0104U,
        __null,
        __null,
        0,
        (void *)__null,
        (sizeof(m_zboss_specific_data.temperature_sensor_clusters) / sizeof(zb_zcl_cluster_desc_t)),
        m_zboss_specific_data.temperature_sensor_clusters,
        (zb_af_simple_desc_1_1_t *)&m_zboss_specific_data.simple_desc_temperature_sensor,
        (1),
        m_zboss_specific_data.reporting_infotemperature_sensor,
        0,
        __null};

    set_desc(&m_zboss_specific_data.temperature_sensor);
    Zigbee::getInstance().addEP(this);
};

void TemperatureSensorCTX::periodic_CB()
{
    float curr_temp = TemperatureSensorCTX::tempCB();
    curr_temp *= 100;
    int16_t curr_temp_cast = (int16_t)curr_temp;

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(TemperatureSensorCTX::ep_id,
                                                     ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE,
                                                     ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
                                                     (zb_uint8_t *)&curr_temp_cast,
                                                     ZB_FALSE);
}

zb_uint8_t TemperatureSensorCTX::endpoint_CB(zb_bufid_t bufid)
{
    return ZB_FALSE; // CB doesn't handle any messages
}
