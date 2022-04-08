#ifndef ZIGBEE_ENDPOINT_IMPLEMENTATION_H_
#define ZIGBEE_ENDPOINT_IMPLEMENTATION_H_

extern "C"
{
#include <stdint.h>
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
}

class ZigbeeEndpoint;
class ZigbeeEndpointImplementation
{
protected:
    ZigbeeEndpointImplementation(ZigbeeEndpoint *interface, const char model_id[], unsigned int power_source_type = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN);

public:
    /** Periodic endpoint update. */
    virtual void update();

    /** Handle a command received from the Zboss stack. */
    virtual zb_uint8_t processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params);

    /** Callback for ep specific events */
    virtual zb_ret_t processCommandDV(zb_zcl_device_callback_param_t *cmd_params);

    /**
     * @brief To be called after the Zigbee stack has been initialized an before starting the normal operations.
     * @param[in] load_from_memory = if the status should be loaded from Zigbee stack persistent memory
     */
    virtual void begin(bool load_from_memory);

    /**
     * @brief To be called to end operations.
     */
    virtual void end();

    /** Identify signal handler */
    virtual void onIdentify(zb_zcl_identify_effect_value_param_t *idt_params);
    /** Leave signal handler */
    virtual void onLeave();

    /** Set the model id attribute **/
    void setModelID(char model_id[]);

    uint8_t endpointId() const;

    zb_af_endpoint_desc_t *endpointDescriptor() const;

    /* Delete the copy constructor. */
    ZigbeeEndpointImplementation(ZigbeeEndpointImplementation const &) = delete;
    /* Delete the assignment operator. */
    void operator=(ZigbeeEndpointImplementation const &) = delete;

protected:
    bool setDescriptor(zb_af_endpoint_desc_t *descriptor);
    /* Check update time for edge cases */
    uint32_t checkUpdatePeriod(zb_uint16_t min_interval, zb_uint16_t max_interval, uint32_t default_period);

    typedef struct
    {
        /** Basic cluster attributes */
        zb_zcl_basic_attrs_ext_t basic_attr;
        /** Identify cluster attributes */
        zb_zcl_identify_attrs_t identify_attr;

        zb_uint16_t cluster_revision_identify_attr_list;
        zb_zcl_attr_t identify_attr_list[3];
        zb_bool_t device_enable_basic_attr_list;
        zb_uint16_t cluster_revision_basic_attr_list;
        zb_zcl_attr_t basic_attr_list[14];
    } ZbossData;

    /** Endpoint identifier: from 1 to 244. */
    const uint8_t m_endpoint_id;

    /** Endpoint description for user applications */
    zb_af_endpoint_desc_t *m_endpoint_descriptor;

    /** Zigbee Zboss stack endpoint basic data */
    ZbossData m_zboss_basic_data;

    /** Backpointer to the visible object. */
    ZigbeeEndpoint *const m_interface;

    /** Set to true if the Zigbee endpoint communication is active. */
    bool m_started;

private:
    /** [1, 240] Endpoint ID counter for automatic endpoint ID generation */
    static uint8_t m_ep_id_counter;
};

#endif
