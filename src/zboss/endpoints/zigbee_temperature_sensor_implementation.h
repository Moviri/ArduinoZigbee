#ifndef ZIGBEE_TEMP_SENSOR_IMPLEMENTATION_H_
#define ZIGBEE_TEMP_SENSOR_IMPLEMENTATION_H_

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "zigbee_endpoint_implementation.h"

class ZigbeeTemperatureSensor;

class ZigbeeTemperatureSensorImplementation : public ZigbeeEndpointImplementation
{
public:
    ZigbeeTemperatureSensorImplementation(ZigbeeTemperatureSensor *interface,
                                          const zb_char_t model_id[] = "Temperature Sensor v1",
                                          unsigned int power_source_type = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN);

    zb_uint8_t processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params) override;
    void update() override;

private:
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
        zb_zcl_temp_measurement_attrs_t temp_attr;

        zb_uint16_t cluster_revision_temperature_attr_list;
        zb_zcl_attr_t temperature_attr_list[6];
        zb_zcl_cluster_desc_t temperature_sensor_clusters[4];
        zb_af_simple_desc_temperature_sensor simple_desc_temperature_sensor;
        zb_zcl_reporting_info_t reporting_infotemperature_sensor[1];
        zb_af_endpoint_desc_t temperature_sensor;
    } ZbossSpecificData;

    ZbossSpecificData m_zboss_data;
    /** Backpointer to the visible object. */
    ZigbeeTemperatureSensor *const m_interface;
};

#endif // ZIGBEE_TEMP_SENSOR_IMPLEMENTATION_H_
