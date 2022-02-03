#ifndef _EP_CTX_H_
#define _EP_CTX_H_

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}

class EndpointCTX
{
public:
    EndpointCTX(float period_p);

    /* Attributes */
    static uint8_t ep_id_counter; // [1, 240] Endpoint ID counter for automatic EP ID generation

    uint8_t ep_id;                         // Endpoint ID
    zb_zcl_basic_attrs_ext_t basic_attr;   // Basic cluster attributes
    zb_zcl_identify_attrs_t identify_attr; // Identify cluster attributes

    float period;                   // Milliseconds between two perodic events
    uint32_t last_trig_time = 0;    // Last time step at which periodic_CB() has been triggered
    zb_af_endpoint_desc_t *ep_desc; // Endpoint description for user applications

    /* Methods */
    virtual void periodic_CB() = 0;                       // Callback for periodic events
    virtual zb_uint8_t endpoint_CB(zb_bufid_t bufid) = 0; // Callback for ep specific events
    bool set_desc(zb_af_endpoint_desc_t *ep_desc_p);      // Setter function for ep_desc attribute

    /* Virtual methods for cluster management */
    virtual void setAttribute(zb_zcl_set_attr_value_param_t *attr_p);
    virtual void setLevelControl(zb_zcl_level_control_set_value_param_t level_p);
};

#endif
