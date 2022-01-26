#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include <vector>

void TemperatureSensorCTX::periodic_CB() {

    Serial.println("CB triggered");
    float curr_temp = TemperatureSensorCTX::tempCB();
    curr_temp *= 100;
    int16_t curr_temp_cast = (int16_t) curr_temp;

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(TemperatureSensorCTX::ep_id, 
                                                     ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                     ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, 
                                                     (zb_uint8_t *)&curr_temp_cast, 
                                                     ZB_FALSE);
}

zb_uint8_t TemperatureSensorCTX::endpoint_CB(zb_bufid_t bufid) {
    return ZB_FALSE;  // CB doesn't handle any messages
}
