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

#include "Arduino.h"
#include "Zigbee.h"
#include "pal_bb.h"
#include "sdksupport/zigbee_device.h"
#include "endpoints/temperature_sensor/zigbee_temperature_sensor.h"

/** Global library helper object reference typical of Arduino.
 * To be used only in the .ino file.
 */
Zigbee &ZIGBEE = Zigbee::getInstance();

extern "C"
{
    int zigbee_init(const zb_uint32_t channelMask);
    int zigbee_start();
    void nrf_802154_core_irq_handler(void);
}

Zigbee& Zigbee::getInstance()
{
    // Instantiate just one instance on the first use.
    static Zigbee instance;
    return instance;
}

std::vector<EndpointCTX*>& Zigbee::endpoints() {
    return m_endpoints;
}

Zigbee::Zigbee(): m_zb_tc_key(nullptr)
{
}

Zigbee::~Zigbee()
{
}

void Zigbee::setTrustCenterKey(zb_uint8_t *zb_tc_key_l)
{
    m_zb_tc_key = zb_tc_key_l;
}

int Zigbee::begin(const zb_uint32_t channelMask)
{
    PalBbSetProtId(BB_PROT_15P4);
    PalBbRegisterProtIrq(BB_PROT_15P4, NULL, nrf_802154_core_irq_handler);
    zigbee_init(channelMask);
    if (m_zb_tc_key != nullptr) {
        zb_zdo_set_tc_standard_distributed_key(m_zb_tc_key);
        zb_zdo_setup_network_as_distributed();
    }
     
    init_device();
    zigbee_start();
}

void Zigbee::end()
{
}

void Zigbee::poll()
{
    zboss_main_loop_iteration();

    // Check if periodic callbacks need to be triggered
    Zigbee::check_periodic_CB();
}

int Zigbee::addEP(EndpointCTX* ep_ctx) 
{
    m_endpoints.push_back(ep_ctx);
    return 0;
}

// CB are triggered period seconds after the previous call. Relative time, not absolute time
void Zigbee::check_periodic_CB()
{
    uint32_t curr_time = millis();
    for(int i=0; i < m_endpoints.size(); i++) {
        EndpointCTX* endpoint = m_endpoints[i];
        if ((endpoint->period > 0) && (curr_time - endpoint->last_trig_time >= endpoint->period)) {
            endpoint->periodic_CB();
            endpoint->last_trig_time = curr_time;
        }
    }
}
