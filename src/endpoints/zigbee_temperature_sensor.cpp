#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include "../zboss/endpoints/zigbee_temperature_sensor_implementation.h"
#include <vector>

ZigbeeTemperatureSensor::ZigbeeTemperatureSensor(ReadTemperatureCallback callback, char model_id[], unsigned int power_source_type) : ZigbeeEndpoint(new ZigbeeTemperatureSensorImplementation(this, model_id, power_source_type)),
                                                                                                                                      m_read_temperature(callback)
{
}

ZigbeeTemperatureSensor::~ZigbeeTemperatureSensor() {}

void ZigbeeTemperatureSensor::update()
{
    m_impl->update();
}

ZigbeeTemperatureSensorImplementation *ZigbeeTemperatureSensor::implementation() const
{
    return static_cast<ZigbeeTemperatureSensorImplementation *>(m_impl.get());
}