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

#ifndef _ZIGBEE_H_
#define _ZIGBEE_H_

#include <vector>
#include "endpoints/endpoint_ctx.h"

/**
 * @brief The Zigbee class is designed to be a singleton object that reprsents the Zigbee stack.
 */
class Zigbee {
private:
  Zigbee();
  ~Zigbee();

public:
  Zigbee(Zigbee const&)          = delete;
  void operator=(Zigbee const&)  = delete;

  static Zigbee& getInstance();

  void setTrustCenterKey(zb_uint8_t *zb_tc_key);
  int begin(const zb_uint32_t channelMask = ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
  void end();
  void poll();
  int addEP(EndpointCTX* ep_ctx);

  static zb_uint8_t endpoint_CB_wrapper(zb_bufid_t bufid);
  static EndpointCTX* getEndpointByID(uint8_t ep_id);

private:
    void update_endpoints();
    void init_device_ctx();
    int init_device();

    zb_uint8_t *m_zb_tc_key;
    std::vector<EndpointCTX*> m_endpoints;
};

extern Zigbee& ZIGBEE;
#endif
