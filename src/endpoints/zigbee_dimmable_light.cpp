#include <mbed.h>
#include "zigbee_dimmable_light.h"
#include "../zboss/endpoints/zigbee_dimmable_light_implementation.h"
#include <vector>

ZigbeeDimmableLight::ZigbeeDimmableLight(WriteBrightnessCallback callback, char model_id[], unsigned int power_source_type) : ZigbeeEndpoint(new ZigbeeDimmableLightImplementation(this, model_id, power_source_type)),
                                                                                                                              m_write_brightness(callback)
{
}

ZigbeeDimmableLight::~ZigbeeDimmableLight() {}

void ZigbeeDimmableLight::setBrightness(unsigned char value)
{
    implementation()->setBrightness(value);
}

void ZigbeeDimmableLight::setState(bool value)
{
    implementation()->setState(value);
}

ZigbeeDimmableLightImplementation *ZigbeeDimmableLight::implementation() const
{
    return static_cast<ZigbeeDimmableLightImplementation *>(m_impl.get());
}