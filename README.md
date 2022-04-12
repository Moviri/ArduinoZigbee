# ArduinoZigbee

Enable Zigbee support on the Arduino Nano 33 BLE.

This library supports creating Zigbee devices with multiple end points that implement the **Zigbee Home Automation profile**.

It requires ArduinoCore-mbed with the specific variant `ARDUINO_NANO33ZIGBEE`.  
At the moment this variant is available on the branch `nordic-zigbee-support` of https://github.com/Moviri/ArduinoCore-mbed/.

## Implemented Home Automation profile devices

- Dimmable Light
- Temperature Sensor

## Implementation notes

See [Zboss endpoints README](src/zboss/endpoints/README.md) for some implementation details that can be useful to add support for other Home Automation devices.

## License

```
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
```
