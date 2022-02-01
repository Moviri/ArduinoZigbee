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

#ifndef _ARDUINO_ZIGBEE_H_
#define _ARDUINO_ZIGBEE_H_

#include "Zigbee.h"
#include "endpoints/dimmable_light/zigbee_dimmable_light.h"
#include "endpoints/temperature_sensor/zigbee_temperature_sensor.h"
#include "endpoints/endpoint_ctx.h"

uint8_t EndpointCTX::ep_id_counter = 0;  // Initialize endpoint id counter

#endif
