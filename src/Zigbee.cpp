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

extern "C"
{
    int zigbee_init(const zb_uint32_t channelMask);
    int zigbee_start();
}

std::vector<EndpointCTX*>& vector_istance() {
    static std::vector<EndpointCTX*> ctx_temp_class_arr;
    return ctx_temp_class_arr;
}

std::vector<uint32_t>& trig_vector_istance() {
    static std::vector<uint32_t> last_trig_time;
    return last_trig_time;
}

Zigbee::Zigbee()
{
    Zigbee::zb_tc_key = NULL;
}

Zigbee::~Zigbee()
{
}

void Zigbee::setTrustCenterKey(zb_uint8_t *zb_tc_key_l)
{
    //zb_zdo_set_tc_standard_distributed_key(zb_tc_key);
    //zb_zdo_setup_network_as_distributed();

    Zigbee::zb_tc_key = zb_tc_key_l;
}

extern "C"{
    void nrf_802154_core_irq_handler(void);
}

int Zigbee::begin(const zb_uint32_t channelMask)
{
    PalBbSetProtId(BB_PROT_15P4);
    PalBbRegisterProtIrq(BB_PROT_15P4, NULL, nrf_802154_core_irq_handler);
    zigbee_init(channelMask);
    if (Zigbee::zb_tc_key != NULL) {
        zb_zdo_set_tc_standard_distributed_key(Zigbee::zb_tc_key);
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
    vector_istance().push_back(ep_ctx);
    trig_vector_istance().push_back(0);
    return 0;
}

// CB are triggered period seconds after the previous call. Relative time, not absolute time
void Zigbee::check_periodic_CB()
{
    uint32_t curr_time = millis();
    for(int i=0; i < vector_istance().size(); i++) {
        if (curr_time - trig_vector_istance()[i] >= vector_istance()[i]->period) {
            vector_istance()[i]->periodic_CB();
            trig_vector_istance()[i] = curr_time;
        }
    }
}

Zigbee ZIGBEEObj;
Zigbee &ZIGBEE = ZIGBEEObj;
