#ifndef ZIGBEE_DEVICE_IMPLEMENTATION_H_
#define ZIGBEE_DEVICE_IMPLEMENTATION_H_

#include "../endpoints/zigbee_endpoint.h"
#include <vector>
extern "C"
{
#include "zboss_api.h"
}

/**
 * @brief The ZigbeeDeviceImplementation class implements the support for the Zboss SDK.
 */
class ZigbeeDeviceImplementation
{
public:
    ZigbeeDeviceImplementation();
    ~ZigbeeDeviceImplementation();

    void setTrustCenterKey(unsigned char *key);

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

    /**
     * @brief Erase peristent memory on startup
     */
    void eraseMemory();

    /**
     * @brief Handle the command that is received when the device has to leave the network.
     */
    void onLeave();

    ZigbeeEndpoint *getEndpointByID(uint8_t id) const;

private:
    void updateEndpoints();
    void initDeviceContext();
    int initDevice();
    zb_bool_t isMemoryToErase();

    unsigned char *m_trust_center_key;
    zb_af_device_ctx_t m_context;
    std::vector<ZigbeeEndpoint *> m_endpoints;
    /** Flag set to true if the memory used by Zboss stack should be erased before starting the stack. */
    zb_bool_t m_erase_persistent_mem;
};

#endif