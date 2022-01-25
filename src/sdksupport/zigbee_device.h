#ifndef ZIGBEE_EP_MANAGER_H__
#define ZIGBEE_EP_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "zboss_api_af.h"
#include "endpoints/endpoint_enum.h"

bool add_EP(zb_af_endpoint_desc_t* ep, ep_type_enum ep_type);
void init_device_ctx();
int init_device();

#ifdef __cplusplus
}
#endif

#endif //ZIGBEE_EP_MANAGER_H__
