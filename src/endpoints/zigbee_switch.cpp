#include <mbed.h>
#include "zigbee_switch.h"
#include "../zboss/endpoints/zigbee_switch_implementation.h"

/** Constructors */

ZigbeeSwitch::ZigbeeSwitch(ReadSwitchPotCallback callback_p, 
                           ReadSwitchButCallback callback_b) : ZigbeeSwitch("Switch v1", 
                                                                            callback_p, 
                                                                            callback_b) {}
ZigbeeSwitch::ZigbeeSwitch(const char model_id[], 
                           ReadSwitchPotCallback callback_p, 
                           ReadSwitchButCallback callback_b) : ZigbeeSwitch(model_id, 
                                                                            ZigbeeEndpoint::SourceType::kDcSource, 
                                                                            callback_p, 
                                                                            callback_b, 
                                                                            nullptr) {}
ZigbeeSwitch::ZigbeeSwitch(const char model_id[], 
                           ZigbeeEndpoint::SourceType power_source_type, 
                           ReadSwitchPotCallback callback_p, 
                           ReadSwitchButCallback callback_b, 
                           ZigbeeEndpoint::IdentifyCallback identify_callback) : ZigbeeEndpoint(new ZigbeeSwitchImplementation(this, 
                                                                                                                               model_id, 
                                                                                                                               static_cast<unsigned int>(power_source_type)), 
                                                                                                                               identify_callback),
                                                                                                                               m_read_switch_p(callback_p),
                                                                                                                               m_read_switch_b(callback_b) {}
/** Deconstructor */

ZigbeeSwitch::~ZigbeeSwitch() {}

/*+ Functions */

void ZigbeeSwitch::update()
{
    m_impl->update();
}

ZigbeeSwitchImplementation *ZigbeeSwitch::implementation() const
{
    return static_cast<ZigbeeSwitchImplementation *>(m_impl.get());
}
