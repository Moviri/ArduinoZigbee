#ifndef ZIGBEE_ZCL_PAYLOAD_READER_H_
#define ZIGBEE_ZCL_PAYLOAD_READER_H_

#include "zboss_api.h"

class ZbossZCLPayloadReader
{
private:
    ZbossZCLPayloadReader();

public:
    static ZbossZCLPayloadReader &getInstance();
    static zb_uint8_t *getPayloadHEX(zb_bufid_t buf_id);

    /* Delete the copy constructor. */
    ZbossZCLPayloadReader(ZbossZCLPayloadReader const &) = delete;
    /* Delete the assignment operator. */
    void operator=(ZbossZCLPayloadReader const &) = delete;
};

#endif // ZIGBEE_ZCL_PAYLOAD_READER_H_
