#ifndef ZIGBEE_EP_ENUM_H__
#define ZIGBEE_EP_ENUM_H__

#include "zboss_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ha_dimmable_light,
    ha_switch,
    ha_temp_sensor
} ep_type_enum;

typedef struct 
{
    ep_type_enum ep_type;
    zb_af_endpoint_desc_t* ep;
} zb_af_endpoint_desc_typed_t;

#ifdef __cplusplus
}
#endif

#endif // ZIGBEE_EP_ENUM_H__
