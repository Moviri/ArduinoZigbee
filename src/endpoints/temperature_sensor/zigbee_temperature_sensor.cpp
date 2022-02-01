#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include <vector>

TemperatureSensorCTX::TemperatureSensorCTX(float period_p, float (*aTempCB)()) : EndpointCTX(period_p)
{
    tempCB = aTempCB;

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
};

void TemperatureSensorCTX::periodic_CB()
{

    // Serial.println("CB triggered");
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
