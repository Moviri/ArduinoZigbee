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
     * @brief Set the Zigbee Revision Level Compatibility
     * Notes:
     * - a revision level below 21 will disable the "Trust Center Link Key update procedure" upon joining the network
     *
     * @param[in] revision = a Zigbee revision number
     * @return 1 if it succeeds.
     */
    int setZigbeeRevisionLevelCompatibility(unsigned int revision);

    /**
     * @brief Start Zigbee communication.
     *        Example: ZIGBEE.begin({11, 12}); // Enable the communication using channel 11 and 12.
     *
     * @param[in] channels = the channels to be used for Zigbee communication. If no channel is set all the channels are used.
     * @return 1 if it succeeds.
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
     * @return 1 if it succeeds.
     */
    int addEndpoint(ZigbeeEndpoint &endpoint);

    /**
     * @brief Perform "Reset with a Local Action" procedure (as described in BDB spec, chapter 9.5).
     *  The device will perform the NLME leave and clean all Zigbee persistent data except the outgoing NWK
     *  frame counter and application datasets (if any).
     *  The reset can be performed at any time once the device is started (see @ref begin()).
     *
     * @return 1 if it succeeds.
     */
    int leaveNetwork();

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
    /** The number of the Zigbee standard revision level the stack should attempt to be compatible with. */
    int m_revision_level_compatibility;
    zb_af_device_ctx_t m_context;
    std::vector<ZigbeeEndpoint *> m_endpoints;
    /** Flag set to true if the memory used by Zboss stack should be erased before starting the stack. */
    zb_bool_t m_erase_persistent_mem;
};

#endif