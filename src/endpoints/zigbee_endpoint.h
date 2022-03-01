#ifndef ZIGBEE_ENDPOINT_H_
#define ZIGBEE_ENDPOINT_H_

#include <stdint.h>
#include <memory>

class ZigbeeEndpointImplementation;

class ZigbeeEndpoint
{
public:
    enum class IdentifyEffect
    {
        /**< Effect identifier field value: Light is turned on/off once */
        kBlink = 0x00,
        /**< Effect identifier field value: Light turned on/off over 1 second and
         * repeated 15 times */
        kBreathe = 0x01,
        /**< Effect identifier field value: Colored light turns green for 1 second;
         * non-colored light flashes twice */
        kOkay = 0x02,
        /**< Effect identifier field value: Colored light turns orange for 8 seconds; non-colored
         * light switches to maximum brightness for 0.5s and then minimum brightness for 7.5s */
        kChannelChange = 0xb,
        /**< Effect identifier field value: Complete the current effect sequence before terminating.
         * E.g., if in the middle of a breathe effect (as above), first complete the current 1s
         * breathe effect and then terminate the effect*/
        kFinishEffect = 0xfe,
        /**< Effect identifier field value: Terminate the effect as soon as possible */
        kStop = 0xff,
    };

    /** The type of the callback to write the value to the Arduino sketch. */
    typedef void (*IdentifyCallback)(IdentifyEffect effect);

    /**
     *  @brief Allowed values for "Power source" attribute.
     *  @see ZCL spec, subclauses 3.2.2.2.9.
     */
    enum class SourceType
    {
        /**< Power source unknown. */
        kUnknown = 0x00,
        /**< Single-phase mains. */
        kMainsSinglePhase = 0x01,
        /**< 3-phase mains. */
        kMainsThreePhase = 0x02,
        /**< Battery source. */
        kBattery = 0x03,
        /**< DC source. */
        kDcSource = 0x04,
        /**< Emergency mains constantly powered. */
        kEmergencyMainsConst = 0x05,
        /**< Emergency mains and transfer switch. */
        kEmergencyMainsTransf = 0x06
    };

protected:
    ZigbeeEndpoint(ZigbeeEndpointImplementation *impl, IdentifyCallback identify_callback = nullptr);
    ~ZigbeeEndpoint();

public:
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

    /** Milliseconds between two perodic events */
    uint32_t m_period;

protected:
    const uint8_t m_endpoint_id;

    /** Last time step at which periodic_CB() has been triggered */
    uint32_t m_last_update_time;

    const IdentifyCallback m_identify_callback;
    /* Pointer to implementation (PIMPL) opaque pointer. */
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
