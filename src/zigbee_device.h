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

#ifndef ZIGBEE_DEVICE_H_
#define ZIGBEE_DEVICE_H_

#include <vector>
#include <memory>

class ZigbeeEndpoint;
class ZigbeeDeviceImplementation;

/**
 * @brief The Zigbee class is designed to be a singleton object that reprsents the Zigbee local device.
 */
class ZigbeeDevice
{
private:
    ZigbeeDevice();
    ~ZigbeeDevice();

public:
    /** The first channel available for Zigbee communication */
    static const unsigned int kFirstChannel = 11;
    /** The last channel available for Zigbee communication */
    static const unsigned int kLastChannel = 25;
    /**
     * @brief Get the unique instance of this class.
     *
     * @return Zigbee&
     */
    static ZigbeeDevice &getInstance();

    void setTrustCenterKey(unsigned char *key);

    /**
     * @brief Start Zigbee communication
     * @param[in] channel = the channel to be used for Zigbee communication. If the channel is not set all the channels are used.
     * @return 0 if it succeeds.
     */
    int begin(const unsigned int channel = 0);

    /**
     * @brief Start Zigbee communication.
     *        Example: ZIGBEE.begin({11, 12}); // Enable the communication using channel 11 and 12.
     *
     * @param[in] channels = the channels to be used for Zigbee communication. If no channel is set all the channels are used.
     * @return 0 if it succeeds.
     */
    int begin(const std::vector<unsigned int> channels = {});

    /**
     * @brief Stop Zigbee communication
     */
    void end();

    /**
     * @brief To be called periodically to let the Zigbee device perform its internal operations.
     */
    void poll();

    /**
     * @brief Add an endpoint to the local Zigbee device so that it will be managed properly.
     *
     * @param[in] endpoint = the endpoint to be managed
     * @return 0 if it succeeds.
     */
    int addEndpoint(ZigbeeEndpoint &endpoint);

    /**
     * @brief Set the Device Name
     *
     * @param model_id = string containing device name
     */
    void setDeviceName(char model_id[]);

    ZigbeeEndpoint *getEndpointByID(unsigned char id) const;

    /* Delete the copy constructor. */
    ZigbeeDevice(ZigbeeDevice const &) = delete;
    /* Delete the assignment operator. */
    void operator=(ZigbeeDevice const &) = delete;

private:
    /* Private implementation (PIMPL) opaque pointer. */
    std::unique_ptr<ZigbeeDeviceImplementation> const m_impl;
    friend class ZigbeeDevice;
};

extern ZigbeeDevice &ZIGBEE;
#endif