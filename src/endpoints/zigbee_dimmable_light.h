#ifndef ZIGBEE_DIM_LIGHT_H__
#define ZIGBEE_DIM_LIGHT_H__

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "zigbee_endpoint.h"

class ZigbeeDimmableLight : public ZigbeeEndpoint
{
public:
    /** The type of the callback to write the value to the Arduino sketch. */
    typedef void (*WriteBrightnessCallback)(const uint8_t brightness_level);

    ZigbeeDimmableLight(WriteBrightnessCallback callback,
                        zb_char_t model_id[] = "Dimmable Light v1",
                        unsigned int power_source_type = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN);

    // Callback for ep specific events
    zb_uint8_t processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params) override;
    // Callback for device events
    zb_ret_t processCommandDV(zb_zcl_device_callback_param_t *cmd_params) override;

    /**
     * @brief Set the light brightness
     * @param[in] brightness = the brightness level.
     */
    void setBrightness(zb_uint8_t brightness);
    /**
     * @brief Switch the lamp ON or OFF
     * @param[in] value = true or false
     */
    void setState(zb_bool_t value);

private:
    zb_uint8_t setAttribute(zb_zcl_set_attr_value_param_t *attr_p);

    typedef struct zb_af_simple_desc_dimmable_light
    {
        zb_uint8_t endpoint;
        zb_uint16_t app_profile_id;
        zb_uint16_t app_device_id;
        zb_bitfield_t app_device_version : 4;
        zb_bitfield_t reserved : 4;
        zb_uint8_t app_input_cluster_count;
        zb_uint8_t app_output_cluster_count;
        zb_uint16_t app_cluster_list[(6) + (0)];
        zb_uint8_t cluster_encryption[((6) + (0) + 7) / 8];
    } __attribute__((packed)) zb_af_simple_desc_dimmable_light;

    typedef struct
    {
        zb_zcl_scenes_attrs_t scenes_attr;
        zb_zcl_groups_attrs_t groups_attr;
        zb_zcl_on_off_attrs_ext_t on_off_attr;
        zb_zcl_level_control_attrs_t level_control_attr;

        zb_uint16_t cluster_revision_scenes_attr_list;
        zb_zcl_attr_t scenes_attr_list[7];
        zb_uint16_t cluster_revision_groups_attr_list;
        zb_zcl_attr_t groups_attr_list[3];
        zb_uint16_t cluster_revision_on_off_attr_list;
        zb_zcl_attr_t on_off_attr_list[6];
        zb_zcl_level_control_move_status_t move_status_data_ctxdim_light;
        zb_uint16_t cluster_revision_level_control_attr_list;
        zb_zcl_attr_t level_control_attr_list[5];
        zb_zcl_cluster_desc_t dimmable_light_clusters[6];
        zb_af_simple_desc_dimmable_light simple_desc;
        zb_zcl_reporting_info_t reporting_infodimmable_light[2];
        zb_zcl_cvc_alarm_variables_t cvc_alarm_infodimmable_light[1];
        zb_af_endpoint_desc_t dimmable_light_ep;
    } ZbossSpecificData;

    /** Callback to write the value to the Arduino sketch. */
    const WriteBrightnessCallback m_write_brightness;
    ZbossSpecificData m_zboss_data;
};

#endif // ZIGBEE_DIM_LIGHT_H__
