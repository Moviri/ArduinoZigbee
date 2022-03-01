#ifndef ZIGBEE_DIM_LIGHT_H__
#define ZIGBEE_DIM_LIGHT_H__

#include "zigbee_endpoint.h"

class ZigbeeDimmableLightImplementation;

class ZigbeeDimmableLight : public ZigbeeEndpoint
{
public:
    /** The type of the callback to write the value to the Arduino sketch. */
    typedef void (*WriteBrightnessCallback)(const uint8_t brightness_level);

    ZigbeeDimmableLight(WriteBrightnessCallback callback);
    ZigbeeDimmableLight(const char model_id[], WriteBrightnessCallback brightness_callback);
    ZigbeeDimmableLight(const char model_id[],
                        ZigbeeEndpoint::SourceType power_source_type,
                        WriteBrightnessCallback brightness_callback,
                        ZigbeeEndpoint::IdentifyCallback identify_callback);
    ~ZigbeeDimmableLight();

    /**
     * @brief Set the light brightness
     * @param[in] brightness = the brightness level.
     */
    void setBrightness(unsigned char brightness);
    /**
     * @brief Switch the lamp ON or OFF
     * @param[in] value = true or false
     */
    void setState(bool value);

    ZigbeeDimmableLightImplementation *implementation() const;

private:
    /** Callback to write the value to the Arduino sketch. */
    const WriteBrightnessCallback m_write_brightness;

    friend class ZigbeeDimmableLightImplementation;
};

#endif // ZIGBEE_DIM_LIGHT_H__
