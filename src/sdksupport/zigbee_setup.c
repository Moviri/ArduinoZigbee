#include "zboss_api.h"
#include "zb_mem_config_med.h"
#include "zb_error_handler.h"

#define MAX_CHILDREN 10                 /**< The maximum amount of connected devices. Setting this value to 0 disables association to this device.  */
#define ERASE_PERSISTENT_CONFIG ZB_FALSE /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. */

void zboss_signal_handler(zb_bufid_t bufid)
{
    /* No application-specific behavior is required. Call default signal handler. */
    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}

int zigbee_init(const zb_uint32_t channelMask)
{
    zb_ieee_addr_t ieee_addr;

    /* Initialize Zigbee stack. */
    ZB_INIT("arduino_device");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    /* Set static long IEEE address. */
    zb_set_network_router_role(channelMask);
    zb_set_max_children(MAX_CHILDREN);
    zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    return 1;
}

int zigbee_start()
{
    zb_ret_t zb_err_code;

    zb_bdb_set_legacy_device_support(0);

    /** Start Zigbee Stack. */
    zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    return 1;
}