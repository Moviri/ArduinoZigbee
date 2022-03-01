#include <mbed.h>
#include "zigbee_dimmable_light.h"
#include "../zboss/endpoints/zigbee_dimmable_light_implementation.h"

ZigbeeDimmableLight::ZigbeeDimmableLight(WriteBrightnessCallback brightness_callback) : ZigbeeDimmableLight("Dimmable Light v1", brightness_callback)
{
}

ZigbeeDimmableLight::ZigbeeDimmableLight(const char model_id[], WriteBrightnessCallback brightness_callback) : ZigbeeDimmableLight(model_id, ZigbeeEndpoint::SourceType::kDcSource, brightness_callback, nullptr)
{
}

ZigbeeDimmableLight::ZigbeeDimmableLight(const char model_id[],
                                         ZigbeeEndpoint::SourceType power_source_type,
                                         WriteBrightnessCallback brightness_callback,
                                         ZigbeeEndpoint::IdentifyCallback identify_callback) : ZigbeeEndpoint(new ZigbeeDimmableLightImplementation(this, model_id, static_cast<unsigned int>(power_source_type)), identify_callback),
                                                                                               m_write_brightness(brightness_callback)
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