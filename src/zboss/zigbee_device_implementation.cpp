#include "Arduino.h"
#include "zigbee_device_implementation.h"
#include "pal_bb.h"

#include "Arduino.h"
extern "C"
{
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_error_handler.h"
#include "zb_mem_config_med.h"
}

#include "../zigbee_device.h"
#include "endpoints/zigbee_endpoint_implementation.h"

#if !defined ZB_ROUTER_ROLE
#error Define ZB_ROUTER_ROLE to compile Router source code.
#endif

/** The maximum amount of connected devices. Setting this value to 0 disables association to this device.  */
#define MAX_CHILDREN 10
/** Do not erase NVRAM to save the network parameters after device reboot or power-off. */                     
#define ERASE_PERSISTENT_CONFIG ZB_TRUE

extern "C"
{
    void nrf_802154_core_irq_handler(void);
    zb_ret_t zigbee_default_signal_handler(zb_bufid_t bufid);
    zb_void_t zigbee_erase_persistent_storage(zb_bool_t erase);
}

void zboss_signal_handler(zb_bufid_t bufid)
{
    /* No application-specific behavior is required. Call default signal handler. */
    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}

static int zigbee_init(const zb_uint32_t channel_mask)
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
    zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    return 1;
}

static zb_ret_t zigbee_start()
{
    zb_bdb_set_legacy_device_support(0);

    /** Start Zigbee Stack. */
    zb_ret_t zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    return 1;
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
}

int ZigbeeDeviceImplementation::initDevice()
{
    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(processZclDeviceCommand);

    /* Register dimmer switch device context (endpoints). */
    initDeviceContext();
    ZB_AF_REGISTER_DEVICE_CTX(&m_context);

    /* register endpoints callback. */
    for (const ZigbeeEndpoint *endpoint : m_endpoints)
    {
        ZB_AF_SET_ENDPOINT_HANDLER(endpoint->endpointId(), processCommandCallback);
    }

    return 1;
}

ZigbeeDeviceImplementation::ZigbeeDeviceImplementation() : m_trust_center_key(nullptr), m_context({0})
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

    PalBbSetProtId(BB_PROT_15P4);
    PalBbRegisterProtIrq(BB_PROT_15P4, NULL, nrf_802154_core_irq_handler);

    zigbee_init((channel_mask == 0) ? ZB_TRANSCEIVER_ALL_CHANNELS_MASK : channel_mask);
    if (m_trust_center_key != nullptr)
    {
        zb_zdo_set_tc_standard_distributed_key(m_trust_center_key);
        zb_zdo_setup_network_as_distributed();
    }

    initDevice();
    return (zigbee_start() == RET_OK) ? 0 : -1;
}

void ZigbeeDeviceImplementation::end()
{
    /* Don't need to do anything special. The user can stop calling poll() at any time. */
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

void ZigbeeDeviceImplementation::setDeviceName(char model_id[])
{
    if (m_endpoints.size() > 0)
    {
        m_endpoints[0]->implementation()->setModelID(model_id);
    }
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