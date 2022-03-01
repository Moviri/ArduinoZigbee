#ifndef ZIGBEE_TEMP_SENS_H__
#define ZIGBEE_TEMP_SENS_H__

#include "zigbee_endpoint.h"

class ZigbeeTemperatureSensorImplementation;

class ZigbeeTemperatureSensor : public ZigbeeEndpoint
{
public:
    /** The ype of the callback to read the value from the Arduino sketch. */
    typedef float (*ReadTemperatureCallback)();

    ZigbeeTemperatureSensor(ReadTemperatureCallback callback);
    ZigbeeTemperatureSensor(const char model_id[], ReadTemperatureCallback callback);
    ZigbeeTemperatureSensor(const char model_id[], ZigbeeEndpoint::SourceType power_source_type, ReadTemperatureCallback temperature_callback, ZigbeeEndpoint::IdentifyCallback identify_callback);
    ~ZigbeeTemperatureSensor();

    void update() override;

    ZigbeeTemperatureSensorImplementation *implementation() const;

private:
    /** Callback to read the value from the Arduino sketch. */
    const ReadTemperatureCallback m_read_temperature;

    friend class ZigbeeTemperatureSensorImplementation;
};

#endif // ZIGBEE_TEMP_SENS_H__
