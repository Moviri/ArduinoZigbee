#include "Arduino.h"
#include "zigbee_device_implementation.h"
#include "pal_bb.h"

extern "C"
{
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
#include "zb_mem_config_med.h"
}

#include "../zigbee_device.h"
#include "endpoints/zigbee_endpoint_implementation.h"
#include "nvram_utils.h"

#if !defined ZB_ROUTER_ROLE
#error Define ZB_ROUTER_ROLE to compile Router source code.
#endif

/** The maximum amount of connected devices. Setting this value to 0 disables association to this device.  */
#define MAX_CHILDREN 10

extern "C"
{
    void nrf_802154_core_irq_handler(void);
    zb_ret_t zigbee_default_signal_handler(zb_bufid_t bufid);
    zb_void_t zigbee_erase_persistent_storage(zb_bool_t erase);
}

/**
 * @brief Mandatory function for all applications implemented on the top of ZBOSS stack.
 * This function should be defined with this specific name, because its is called by the ZBOSS stack where it is declared as extern.
 *
 * @param bufid
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t *p_sg_p = nullptr;
    zb_zdo_app_signal_type_t signal = zb_get_app_signal(bufid, &p_sg_p);

    /* Call default mbed-OS signal handler. It internally calls bdb_start_top_level_commissioning() when ZB_ZDO_SIGNAL_SKIP_STARTUP is detected.  */
    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

    /* Inform our classes about some specific signals. */
    switch (signal)
    {
    case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
        /* The device_annce is provided to enable ZigBee devices on the network to notify other ZigBee devices that the device has joined or re-joined the network. */
        break;
    case ZB_ZDO_SIGNAL_LEAVE:
        /* The device itself has left the network. */
        ZigbeeDevice::getInstance().implementation()->onLeave();
        break;
    case ZB_ZDO_SIGNAL_ERROR:
        /* Some mess in the buffer. */
        break;
    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        /* Device started and commissioned first time after NVRAM erase. The signal has Code + Status only. */
        break;
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        /* BDB initialization completed after device reboot, use NVRAM contents during initialization. Device joined/rejoined and started. The signal has Code + Status only. */
        break;
    case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
        /* Notifies application that application specific part of production configuration can be applied. */
        break;
    default:
        break;
    }

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}

static int zigbee_init(const zb_uint32_t channel_mask, zb_bool_t erase_mem)
{
    zb_ieee_addr_t ieee_addr;

    /* Initialize Zigbee stack. */
    ZB_INIT("arduino_device");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    /* Set static long IEEE address. */
    zb_set_network_router_role(channel_mask);
    zb_set_max_children(MAX_CHILDREN);
    zb_set_nvram_erase_at_start(erase_mem);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    return 1;
}

static zb_ret_t zigbee_start()
{
    /** Start Zigbee Stack. */
    zb_ret_t zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    return zb_err_code;
}

static zb_void_t processZclDeviceCommand(zb_bufid_t bufid)
{
    zb_zcl_device_callback_param_t *p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);
    ZigbeeEndpoint *endpoint = ZigbeeDevice::getInstance().getEndpointByID(p_device_cb_param->endpoint);
    if (endpoint == nullptr)
    {
        p_device_cb_param->status = RET_ERROR;
        return;
    }
    p_device_cb_param->status = endpoint->implementation()->processCommandDV(p_device_cb_param);
}

static zb_uint8_t processCommandCallback(zb_bufid_t bufid)
{
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);
    if (cmd_info == nullptr)
    {
        return ZB_FALSE;
    }

    ZigbeeEndpoint *endpoint = ZigbeeDevice::getInstance().getEndpointByID(cmd_info->addr_data.common_data.dst_endpoint);
    if (endpoint != nullptr)
    {
        if (endpoint->implementation()->processCommandEP(bufid, cmd_info))
        {
            zb_buf_free(bufid);
            return ZB_TRUE;
        }
    }
    return ZB_FALSE;
}

void ZigbeeDeviceImplementation::onLeave()
{
    for (const ZigbeeEndpoint *endpoint : m_endpoints)
    {
        endpoint->implementation()->onLeave();
    }
}

ZigbeeEndpoint *ZigbeeDeviceImplementation::getEndpointByID(uint8_t id) const
{
    for (ZigbeeEndpoint *endpoint : m_endpoints)
    {
        if (endpoint->endpointId() == id)
        {
            return endpoint;
        }
    }
    return nullptr;
}

void ZigbeeDeviceImplementation::initDeviceContext()
{
    m_context.ep_count = m_endpoints.size();
    zb_af_endpoint_desc_t **ep_desc_arr = new zb_af_endpoint_desc_t *[m_context.ep_count];

    int i = 0;
    for (const ZigbeeEndpoint *endpoint : m_endpoints)
    {
        ep_desc_arr[i++] = endpoint->implementation()->endpointDescriptor();
    }
    m_context.ep_desc_list = ep_desc_arr;

    /* Note that pointers to the descriptors (and the pointers to other Zboss related structures inside the descriptors themeselves)
     * will be saved in Zboss NVRAM and reused after reboot if the NVRAM is not cleared.
     * Therefore when the address of the related variables changes the NVRAM should be cleared.
     */
    ZB_AF_REGISTER_DEVICE_CTX(&m_context);
}

int ZigbeeDeviceImplementation::initDevice()
{
    if (m_revision_level_compatibility > 0)
    {
        /* R21 devices joining a Zigbee 3.0 centralized network must initiate a TC Link Key update procedure upon
         * joining the network. This unique TC Link Key is used for all encrypted APS-layer communication instead of
         * the well-known counterpart. The Node Descriptor packet that is sent during network association procedure
         * indicates the joining device's Zigbee version. R21 coordinators (acting as a trust center) can be configured
         * to accept or reject legacy devices that do not initiate the TC Link Key update procedure.
         *
         * The following call with argument 1 is required if the Controller does not support revision 21 or later
         * and will disable "TC link key exchange". However this will make the device not compliant with Zigbee R21 or later.
         */
        zb_bdb_set_legacy_device_support(m_revision_level_compatibility < 21 ? 1 : 0);
    }
    else
    {
        /* Handle default compatibility. */
        /* Enable "TC link key exchange". */
        zb_bdb_set_legacy_device_support(0);
    }

    if (m_trust_center_key != nullptr)
    {
        zb_zdo_set_tc_standard_distributed_key(m_trust_center_key);
        zb_zdo_setup_network_as_distributed();
    }

    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(processZclDeviceCommand);

    /* Register the contexts of every endpoint. */
    initDeviceContext();

    /* Register endpoints callback. */
    for (const ZigbeeEndpoint *endpoint : m_endpoints)
    {
        ZB_AF_SET_ENDPOINT_HANDLER(endpoint->endpointId(), processCommandCallback);
    }

    return 1;
}

ZigbeeDeviceImplementation::ZigbeeDeviceImplementation() : m_trust_center_key(nullptr), m_revision_level_compatibility(0), m_context({0}), m_erase_persistent_mem(ZB_FALSE)
{
}

ZigbeeDeviceImplementation::~ZigbeeDeviceImplementation()
{
    end();
    if (m_context.ep_desc_list != nullptr)
    {
        delete m_context.ep_desc_list;
    }
}

void ZigbeeDeviceImplementation::setTrustCenterKey(unsigned char *key)
{
    m_trust_center_key = key;
}

int ZigbeeDeviceImplementation::setZigbeeRevisionLevelCompatibility(unsigned int revision)
{
    m_revision_level_compatibility = revision;
    return 0;
}

int ZigbeeDeviceImplementation::begin(const std::vector<unsigned int> channels)
{
    unsigned int channel_mask = 0;
    for (const auto &channel : channels)
    {
        if (channel >= ZigbeeDevice::kFirstChannel && channel <= ZigbeeDevice::kLastChannel)
        {
            channel_mask |= 1 << channel;
        }
    }

    const bool clear_persistent_memory = isMemoryToErase() || isSketchChanged();
    zigbee_init((channel_mask == 0) ? ZB_TRANSCEIVER_ALL_CHANNELS_MASK : channel_mask, clear_persistent_memory);

    initDevice();

    int start_ok = (zigbee_start() == RET_OK) ? 0 : -1;
    if ((start_ok == 0) && !clear_persistent_memory)
    {
        for (ZigbeeEndpoint *endpoint : m_endpoints)
        {
            endpoint->implementation()->reloadSettingsFromMemory();
        }
    }

    /* @todo: should we call PalBbInit() ? */

    /* Set the protocol to be handled by the radio driver.
     * Notes:
     * - only one protocol can be used at the same time (see PalBbRegisterProtIrq() implementation in pal_bb.c).
     * - Zigbee uses 802.15.4 MAC layer.
     */
    PalBbSetProtId(BB_PROT_15P4);

    /* Register the callback to be called by the driver when the Zigbee protocol is active and an interrupt is received. */
    PalBbRegisterProtIrq(BB_PROT_15P4, nullptr, nrf_802154_core_irq_handler);

    /* @todo: should we call PalBbEnable() ? */

    return start_ok;
}

void ZigbeeDeviceImplementation::end()
{
    /* Inform the radio driver that we are not using Zigbee anymore. */
    PalBbSetProtId(BB_PROT_NONE);
    PalBbRegisterProtIrq(BB_PROT_15P4, nullptr, nullptr);
    /* @todo: should we call PalBbDisable() ? */

    /* Deregister the callbacks. */
    ZB_ZCL_REGISTER_DEVICE_CB(nullptr);
    for (const ZigbeeEndpoint *endpoint : m_endpoints)
    {
        ZB_AF_SET_ENDPOINT_HANDLER(endpoint->endpointId(), nullptr);
    }
}

void ZigbeeDeviceImplementation::poll()
{
    zboss_main_loop_iteration();

    /* Check if periodic callbacks need to be triggered */
    updateEndpoints();
}

int ZigbeeDeviceImplementation::addEndpoint(ZigbeeEndpoint &endpoint)
{
    m_endpoints.push_back(&endpoint);
    return 0;
}

int ZigbeeDeviceImplementation::leaveNetwork()
{
    /*  The reset can be performed at any time once the device is started (see @ref zboss_start).
     *  After the reset, the application will receive the @ref ZB_ZDO_SIGNAL_LEAVE signal.
     */
    zb_bdb_reset_via_local_action(0);
    return 0;
}

void ZigbeeDeviceImplementation::setDeviceName(char model_id[])
{
    if (m_endpoints.size() > 0)
    {
        m_endpoints[0]->implementation()->setModelID(model_id);
    }
}

void ZigbeeDeviceImplementation::eraseMemory()
{
    m_erase_persistent_mem = ZB_TRUE;
}

zb_bool_t ZigbeeDeviceImplementation::isMemoryToErase()
{
    return m_erase_persistent_mem;
}

void ZigbeeDeviceImplementation::updateEndpoints()
{
    // CB are triggered period seconds after the previous call. Relative time, not absolute time.
    const uint32_t curr_time = millis();
    for (ZigbeeEndpoint *endpoint : m_endpoints)
    {
        endpoint->poll(curr_time);
    }
}