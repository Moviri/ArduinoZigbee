#ifndef _ZIGBEE_SETUP_
#define _ZIGBEE_SETUP_

#ifdef __cplusplus
extern "C"
{
#endif

#include "zboss_api.h"

int zigbee_init(const zb_uint32_t channel_mask);
zb_ret_t zigbee_start();

#ifdef __cplusplus
}
#endif
#endif
