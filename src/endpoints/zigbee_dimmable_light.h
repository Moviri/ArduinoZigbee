#ifndef ZIGBEE_DIM_LIGHT_H__
#define ZIGBEE_DIM_LIGHT_H__

#include "zigbee_endpoint.h"

class ZigbeeDimmableLightImplementation;

class ZigbeeDimmableLight : public ZigbeeEndpoint
{
public:
    /** The type of the callback to write the value to the Arduino sketch. */
    typedef void (*WriteBrightnessCallback)(const uint8_t brightness_level);

    ZigbeeDimmableLight(WriteBrightnessCallback callback,
                        char model_id[] = "Dimmable Light v1",
                        unsigned int power_source_type = 0);
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
