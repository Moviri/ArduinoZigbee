#ifndef _EP_CTX_H_
#define _EP_CTX_H_

#include <stdint.h>
#include <memory>

class ZigbeeEndpointImplementation;

class ZigbeeEndpoint
{
public:
    ZigbeeEndpoint(ZigbeeEndpointImplementation *impl);
    ~ZigbeeEndpoint();

    /** Inform the endpoint about the current time so that it can perform its update. */
    void poll(uint32_t current_time);
    /** Periodic endpoint update. */
    virtual void update();

    uint8_t endpointId() const;
    uint32_t period() const;
    uint32_t lastUpdateTime() const;

    ZigbeeEndpointImplementation *implementation() const;

    /* Delete the copy constructor. */
    ZigbeeEndpoint(ZigbeeEndpoint const &) = delete;
    /* Delete the assignment operator. */
    void operator=(ZigbeeEndpoint const &) = delete;

protected:
    /** [1, 240] Endpoint ID counter for automatic EP ID generation */
    static uint8_t m_ep_id_counter;

    const uint8_t m_endpoint_id;

    /** Milliseconds between two perodic events */
    uint32_t m_period;
    /** Last time step at which periodic_CB() has been triggered */
    uint32_t m_last_update_time;
    /* Private implementation (PIMPL) opaque pointer. */
    std::unique_ptr<ZigbeeEndpointImplementation> const m_impl;
    friend class ZigbeeEndpointImplementation;
};

inline void ZigbeeEndpoint::poll(uint32_t current_time)
{
    if (current_time - m_last_update_time >= m_period)
    {
        m_last_update_time = current_time;
        update();
    }
}

#endif
