#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include "../zboss/endpoints/zigbee_temperature_sensor_implementation.h"

ZigbeeTemperatureSensor::ZigbeeTemperatureSensor(ReadTemperatureCallback temperature_callback) : ZigbeeTemperatureSensor("Temperature Sensor v1", temperature_callback) {}
ZigbeeTemperatureSensor::ZigbeeTemperatureSensor(const char model_id[], ReadTemperatureCallback temperature_callback) : ZigbeeTemperatureSensor(model_id, ZigbeeEndpoint::SourceType::kDcSource, temperature_callback, nullptr) {}
ZigbeeTemperatureSensor::ZigbeeTemperatureSensor(const char model_id[], ZigbeeEndpoint::SourceType power_source_type, ReadTemperatureCallback temperature_callback, ZigbeeEndpoint::IdentifyCallback identify_callback) : ZigbeeEndpoint(new ZigbeeTemperatureSensorImplementation(this, model_id, static_cast<unsigned int>(power_source_type)), identify_callback),
                                                                                                                                                                                                                          m_read_temperature(temperature_callback)
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