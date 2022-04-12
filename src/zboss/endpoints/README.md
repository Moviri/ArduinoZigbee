# How to add new devices
This library leverages the *nRF5 SDK for Thread and Zigbee v4.2.0* to implement Zigbee functionalities. To adapt the SDK to the Arduino world, some macros provided by the SDK have been expanded and added directly to the code.  
To add a new device to the library, the fastest way is to take the SDK examples and export the preprocessor output, to see the device initialization expanded without macro. To do so a great tool is Segger Embedded Studio.  
The structures defining the device can be transposed to our library following the schema of other devices (e.g. Dimmable Light). 
The major structures to copy are:
- Device attribute list
- Device cluster list
- Device simple descriptor

These structures are splitted between
- zigbee_\<device>_implementation.h (Definition)
  - zb_af_simple_desc_\<device>
  - ZbossSpecificData
- zigbee_\<device>_implementation.cpp (Initialization)
  - m_zboss_data.\<*>_attr_list
  - m_zboss_data.\<*>_clusters
  - m_zboss_data.simple_desc_\<device>
  - m_zboss_data.\<device>

Usually, these structures can be found in the preprocessor output and can be copied as they are into the library.
A good example to follow for adding new devices is the dimmable light implementation.

If no Nordic example is available, the device can be defined with the appropriate macros and then compiled saving the preprocessor output.