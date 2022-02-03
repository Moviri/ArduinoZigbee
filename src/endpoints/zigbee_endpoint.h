#ifndef _EP_CTX_H_
#define _EP_CTX_H_

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}

class ZigbeeEndpoint
{
public:
    ZigbeeEndpoint(zb_char_t model_id[], unsigned int power_source_type = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN);

    /** Inform the endpoint about the current time so that it can perform its update. */
    void poll(uint32_t current_time);
    /** Periodic endpoint update. */
    virtual void update();
    /** Handle a command received from the Zboss stack. */
    virtual zb_uint8_t processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params);
    /** Callback for ep specific events. */
    virtual zb_ret_t processCommandDV(zb_zcl_device_callback_param_t *cmd_params);
    /** Identify effect handler **/
    virtual void feedbackEffect(zb_zcl_identify_effect_value_param_t *idt_params);
    /** Setter for model id attribute **/
    void setModelID(zb_char_t model_id[]);

    uint8_t endpointId() const;
    zb_af_endpoint_desc_t *endpointDescriptor() const;
    uint32_t period() const;
    uint32_t lastUpdateTime() const;

    /* Delete the copy constructor. */
    ZigbeeEndpoint(ZigbeeEndpoint const &) = delete;
    /* Delete the assignment operator. */
    void operator=(ZigbeeEndpoint const &) = delete;

protected:
    bool setDescriptor(zb_af_endpoint_desc_t *ep_desc_p); // Setter function for ep_desc attribute

    typedef struct
    {
        zb_zcl_basic_attrs_ext_t basic_attr;   // Basic cluster attributes
        zb_zcl_identify_attrs_t identify_attr; // Identify cluster attributes

        zb_uint16_t cluster_revision_identify_attr_list;
        zb_zcl_attr_t identify_attr_list[3];
        zb_bool_t device_enable_basic_attr_list;
        zb_uint16_t cluster_revision_basic_attr_list;
        zb_zcl_attr_t basic_attr_list[14];
    } ZbossData;

    static uint8_t m_ep_id_counter; // [1, 240] Endpoint ID counter for automatic EP ID generation

    const uint8_t m_endpoint_id;

    uint32_t m_period;                            // Milliseconds between two perodic events
    uint32_t m_last_update_time;                  // Last time step at which periodic_CB() has been triggered
    zb_af_endpoint_desc_t *m_endpoint_descriptor; // Endpoint description for user applications

    ZbossData m_zboss_basic_data;
};

inline void ZigbeeEndpoint::poll(uint32_t current_time)
{
    if ((m_period > 0) && (current_time - m_last_update_time >= m_period))
    {
        m_last_update_time = current_time;
        update();
    }
}

#endif
