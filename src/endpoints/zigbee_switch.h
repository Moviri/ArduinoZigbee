#ifndef ZIGBEE_SWITCH_H__
#define ZIGBEE_SWITCH_H__

#include "zigbee_endpoint.h"

class ZigbeeSwitchImplementation;

class ZigbeeSwitch : public ZigbeeEndpoint
{
public:
    /** The types of the callbacks to read values from the Arduino sketch. */
    typedef bool (*ReadSwitchPotCallback)();
    typedef bool (*ReadSwitchButCallback)();

    ZigbeeSwitch(ReadSwitchPotCallback callback_p, ReadSwitchButCallback callback_b);
    ZigbeeSwitch(const char model_id[], ReadSwitchPotCallback callback_p, ReadSwitchButCallback callback_b);
    ZigbeeSwitch(const char model_id[], ZigbeeEndpoint::SourceType power_source_type, ReadSwitchPotCallback scallback_p, ReadSwitchButCallback callback_b, ZigbeeEndpoint::IdentifyCallback identify_callback);
    ~ZigbeeSwitch();

    void update() override;

    ZigbeeSwitchImplementation *implementation() const;

private:
    /** Callback to read the value from the Arduino sketch. */
    const ReadSwitchPotCallback m_read_switch_p;
    const ReadSwitchButCallback m_read_switch_b;

    friend class ZigbeeSwitchImplementation;
};

#endif // ZIGBEE_SWITCH_H__
