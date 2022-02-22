#include "Arduino.h"
#include "zcl_payload_reader.h"
#include "zcl/zb_zcl_common.h"

ZbossZCLPayloadReader::ZbossZCLPayloadReader()
{

}

ZbossZCLPayloadReader &ZbossZCLPayloadReader::getInstance()
{
    // Instantiate just one instance on the first use.
    static ZbossZCLPayloadReader instance;
    return instance;
}

zb_uint8_t *ZbossZCLPayloadReader::getPayloadHEX(zb_bufid_t bufid)
{
    size_t payload_length = zb_buf_len(bufid);
    const zb_uint8_t *p_payload = (const zb_uint8_t*)zb_buf_begin(bufid);

    for(int i; i<payload_length; i++) {
        Serial.print(*(p_payload+(i*sizeof(zb_uint8_t))), HEX);
    }
    Serial.println();
}
