#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include <vector>

// std::vector<TemperatureSensorCTX*>& vector_istance_t() {
//     static std::vector<TemperatureSensorCTX*> ctx_temp_class_arr;
//     return ctx_temp_class_arr;
// }

// std::vector<uint32_t>& trig_vector_istance_t() {
//     static std::vector<uint32_t> last_trig_time;
//     return last_trig_time;
// }

// TemperatureSensorCTX* get_temp_ctx_by_id(uint8_t ep_id) {
//     for(TemperatureSensorCTX* t : vector_istance_t()) {
//         if (t->ep_id == ep_id) {
//             return t;
//         }
//     }
// }

bool initialize_temp_sensor_EP(TemperatureSensorCTX* p_temp_ctx) {
    vector_istance_t().push_back(p_temp_ctx);
    trig_vector_istance_t().push_back(0);
    return 1;
}

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

}
