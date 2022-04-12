#ifndef ZIGBEE_SWITCH_IMPLEMENTATION_H_
#define ZIGBEE_SWITCH_IMPLEMENTATION_H_

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}
#include "zigbee_endpoint_implementation.h"

class ZigbeeSwitch;

class ZigbeeSwitchImplementation : public ZigbeeEndpointImplementation
{
public:
    ZigbeeSwitchImplementation(ZigbeeSwitch *interface,
                               const zb_char_t model_id[] = "Switch v1",
                               unsigned int power_source_type = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN);

    zb_ret_t processCommandDV(zb_zcl_device_callback_param_t *cmd_params) override;
    void update() override;
    void sendLevelMsg(int brightness);
    void sendOnOffMsg(bool state);

private:
    uint8_t prev_val_p = 0;
    bool prev_val_b = false;

    typedef struct zb_af_simple_desc_switch
    {
        zb_uint8_t endpoint;
        zb_uint16_t app_profile_id;
        zb_uint16_t app_device_id;
        zb_bitfield_t app_device_version : 4;
        zb_bitfield_t reserved : 4;
        zb_uint8_t app_input_cluster_count;
        zb_uint8_t app_output_cluster_count;
        zb_uint16_t app_cluster_list[(2) + (5)];
        zb_uint8_t cluster_encryption[((2) + (5) + 7) / 8];
    } __attribute__((packed)) zb_af_simple_desc_switch;

    typedef struct
    {
        zb_zcl_cluster_desc_t switch_clusters[7];
        zb_af_simple_desc_switch simple_desc;
        zb_af_endpoint_desc_t switch_ep;
    } ZbossSpecificData;

    ZbossSpecificData m_zboss_data;
};

#endif // ZIGBEE_SWITCH_IMPLEMENTATION_H_
