#ifndef ZIGBEE_TEMP_SENS_H__
#define ZIGBEE_TEMP_SENS_H__

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "../endpoint_ctx.h"

class TemperatureSensorCTX : public EndpointCTX
{
public:
    TemperatureSensorCTX(float period_p, float (*aTempCB)());

    zb_zcl_temp_measurement_attrs_t temp_attr;

    float (*tempCB)();                        // Callback defined in Arduino sketch
    void periodic_CB();                       // Periodic callback
    zb_uint8_t endpoint_CB(zb_bufid_t bufid); // Endpoint specific callback

    typedef struct zb_af_simple_desc_temperature_sensor
    {
        zb_uint8_t endpoint;
        zb_uint16_t app_profile_id;
        zb_uint16_t app_device_id;
        zb_bitfield_t app_device_version : 4;
        zb_bitfield_t reserved : 4;
        zb_uint8_t app_input_cluster_count;
        zb_uint8_t app_output_cluster_count;
        zb_uint16_t app_cluster_list[(3) + (1)];
        zb_uint8_t cluster_encryption[((3) + (1) + 7) / 8];
    } __attribute__((packed)) zb_af_simple_desc_temperature_sensor;

    typedef struct
    {
        zb_uint16_t cluster_revision_temperature_attr_list;
        zb_zcl_attr_t temperature_attr_list[6];
        zb_zcl_cluster_desc_t temperature_sensor_clusters[4];
        zb_af_simple_desc_temperature_sensor simple_desc_temperature_sensor;
        zb_zcl_reporting_info_t reporting_infotemperature_sensor[1];
        zb_af_endpoint_desc_t temperature_sensor;
    } ZbossSpecificData;

    ZbossSpecificData m_zboss_specific_data;
};

#define TemperatureSensor(CB) \
    TemperatureSensorCTX temp_sens_##CB(30000, CB);

#endif //ZIGBEE_TEMP_SENS_H__
