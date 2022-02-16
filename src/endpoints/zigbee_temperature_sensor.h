#ifndef ZIGBEE_TEMP_SENS_H__
#define ZIGBEE_TEMP_SENS_H__

#include "zigbee_endpoint.h"

class ZigbeeTemperatureSensorImplementation;

class ZigbeeTemperatureSensor : public ZigbeeEndpoint
{
public:
    /** The ype of the callback to read the value from the Arduino sketch. */
    typedef float (*ReadTemperatureCallback)();

    ZigbeeTemperatureSensor(ReadTemperatureCallback callback,
                            const char model_id[] = "Temperature Sensor v1",
                            unsigned int power_source_type = 0);
    ~ZigbeeTemperatureSensor();

    void update() override;

    ZigbeeTemperatureSensorImplementation *implementation() const;

private:
    /** Callback to read the value from the Arduino sketch. */
    const ReadTemperatureCallback m_read_temperature;

    friend class ZigbeeTemperatureSensorImplementation;
};

#endif // ZIGBEE_TEMP_SENS_H__
