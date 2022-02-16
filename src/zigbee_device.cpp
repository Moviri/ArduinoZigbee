/*
  This file is part of the ArduinoZigbee library.
  Copyright (c) 2021 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "zigbee_device.h"

#include "Arduino.h"
#include "endpoints/zigbee_endpoint.h"
#include "zboss/zigbee_device_implementation.h"

/** Global library helper object reference typical of Arduino.
 * To be used only in the .ino file.
 */
ZigbeeDevice &ZIGBEE = ZigbeeDevice::getInstance();

ZigbeeEndpoint *ZigbeeDevice::getEndpointByID(unsigned char id) const
{
    return m_impl->getEndpointByID(id);
}

ZigbeeDevice &ZigbeeDevice::getInstance()
{
    // Instantiate just one instance on the first use.
    static ZigbeeDevice instance;
    return instance;
}

ZigbeeDevice::ZigbeeDevice() : m_impl(new ZigbeeDeviceImplementation())
{
}

ZigbeeDevice::~ZigbeeDevice()
{
    end();
}

void ZigbeeDevice::setTrustCenterKey(unsigned char *key)
{
    m_impl->setTrustCenterKey(key);
}

int ZigbeeDevice::begin(const unsigned int channel)
{
    return m_impl->begin(std::vector<unsigned int> {channel});
}

int ZigbeeDevice::begin(const std::vector<unsigned int> channels)
{
    return m_impl->begin(channels);
}

void ZigbeeDevice::end()
{
    m_impl->end();
}

void ZigbeeDevice::poll()
{
    m_impl->poll();
}

int ZigbeeDevice::addEndpoint(ZigbeeEndpoint &endpoint)
{
    return m_impl->addEndpoint(endpoint);
}

void ZigbeeDevice::setDeviceName(char model_id[])
{
    m_impl->setDeviceName(model_id);
}