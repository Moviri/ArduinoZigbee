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
#include "ZigbeeEndpoint.h"
#include "endpoints/endpoint_ctx.h"

class Zigbee {
public:
  Zigbee();
  virtual ~Zigbee();

  virtual void setTrustCenterKey(zb_uint8_t *zb_tc_key);
  virtual int begin(const zb_uint32_t channelMask = ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
  virtual void end();
  virtual void poll();
  int addEP(EndpointCTX* ep_ctx);
  
protected:

private:
    virtual void check_periodic_CB();

    zb_uint8_t *zb_tc_key;
    
};

extern Zigbee& ZIGBEE;
#endif
