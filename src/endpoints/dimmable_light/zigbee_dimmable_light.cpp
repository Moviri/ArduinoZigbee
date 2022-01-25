#include <mbed.h>
#include "zigbee_dimmable_light.h"

/* Max sizes */
#define MAX_EP_DIMMABLE_LIGHT 4

/* Basic cluster attributes initial values. */
#define BULB_INIT_BASIC_APP_VERSION       01                                    /**< Version of the application software (1 byte). */
#define BULB_INIT_BASIC_STACK_VERSION     10                                    /**< Version of the implementation of the Zigbee stack (1 byte). */
#define BULB_INIT_BASIC_HW_VERSION        11                                    /**< Version of the hardware of the device (1 byte). */
#define BULB_INIT_BASIC_MANUF_NAME        "Arduino"                             /**< Manufacturer name (32 bytes). */
#define BULB_INIT_BASIC_MODEL_ID          "Dimmable Light v1"                   /**< Model number assigned by manufacturer (32-bytes long string). */
#define BULB_INIT_BASIC_DATE_CODE         "20210911"                            /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define BULB_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE   /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define BULB_INIT_BASIC_LOCATION_DESC     "Generic"                             /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define BULB_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED          /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */


bulb_device_ctx_t *ctx_light_arr[MAX_EP_DIMMABLE_LIGHT];
int current_light_size = 0;

bulb_device_ctx_t* get_light_ctx_by_id(uint8_t ep_id) {
    for (uint8_t i = 0; i < current_light_size; i++)
    {
        if (ctx_light_arr[i]->ep_id == ep_id) {
            return ctx_light_arr[i];
        }
    }
}

bool initialize_light_EP(bulb_device_ctx_t* p_light_ctx, uint8_t ep_id, void (*aBulbCB)(const uint8_t brightness_level)) {
    // Clusters initialization
    memset(p_light_ctx, 0, sizeof(bulb_device_ctx_t));
    p_light_ctx->ep_id = ep_id;
    light_clusters_attr_init(p_light_ctx);

    // Callback
    p_light_ctx->bulbCB = aBulbCB;                          // Arduino sketch callback

    // Context registration
    ctx_light_arr[current_light_size] = p_light_ctx;
    current_light_size++;                                   // Update number of dimmable lights

    return 1;
}

static void zb_set_brightness(zb_uint8_t brightness_level, uint8_t ep_id) 
{
    for (uint8_t i = 0; i < current_light_size; i++)
    {
        if (ctx_light_arr[i]->ep_id == ep_id) {
            ctx_light_arr[i]->bulbCB(brightness_level);
            break;
        }    
    }
}

void light_clusters_attr_init(bulb_device_ctx_t * p_light_ctx)
{
    /* Basic cluster attributes data */
    p_light_ctx->basic_attr.zcl_version   = ZB_ZCL_VERSION;
    p_light_ctx->basic_attr.app_version   = BULB_INIT_BASIC_APP_VERSION;
    p_light_ctx->basic_attr.stack_version = BULB_INIT_BASIC_STACK_VERSION;
    p_light_ctx->basic_attr.hw_version    = BULB_INIT_BASIC_HW_VERSION;

    ZB_ZCL_SET_STRING_VAL(p_light_ctx->basic_attr.mf_name,
                          BULB_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(p_light_ctx->basic_attr.model_id,
                          BULB_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(p_light_ctx->basic_attr.date_code,
                          BULB_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));

    p_light_ctx->basic_attr.power_source = BULB_INIT_BASIC_POWER_SOURCE;

    ZB_ZCL_SET_STRING_VAL(p_light_ctx->basic_attr.location_id,
                          BULB_INIT_BASIC_LOCATION_DESC,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_DESC));


    p_light_ctx->basic_attr.ph_env = BULB_INIT_BASIC_PH_ENV;

    /* Identify cluster attributes data */
    p_light_ctx->identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* On/Off cluster attributes data */
    p_light_ctx->on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;

    /* Level control cluster attributes data */
    p_light_ctx->level_control_attr.current_level  = ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE; // Set current level value to maximum
    p_light_ctx->level_control_attr.remaining_time = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;

    // Removed on_off and current_level attributes initialization. Otherwise program execution enters a loop in "zb_nrf52_abort"

    // ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id, 
    //                      ZB_ZCL_CLUSTER_ID_ON_OFF,    
    //                      ZB_ZCL_CLUSTER_SERVER_ROLE,  
    //                      ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
    //                      (zb_uint8_t *)&p_light_ctx->on_off_attr.on_off,                        
    //                      ZB_FALSE);                   

    // ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,                                       
    //                      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,            
    //                      ZB_ZCL_CLUSTER_SERVER_ROLE,                 
    //                      ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 
    //                      (zb_uint8_t *)&p_light_ctx->level_control_attr.current_level,                                       
    //                      ZB_FALSE);

    // zb_light_set_level(p_light_ctx, p_light_ctx->level_control_attr.current_level);  // Sets light to latest value
}

zb_uint8_t zcl_light_endpoint_cb(zb_bufid_t bufid) {
    zb_bufid_t zcl_cmd_buf = bufid;
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf, zb_zcl_parsed_hdr_t);

    if (    cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV && 
            !cmd_info->is_common_command &&
            cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF &&
            cmd_info->cmd_id == ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID) 
    {
        zb_light_set_state(cmd_info->addr_data.common_data.dst_endpoint, (zb_bool_t)false);

        zb_buf_free(bufid);

        return ZB_TRUE;
    }

    return ZB_FALSE;
}

zb_ret_t zb_light_set_attribute(uint8_t ep_id,
                                zb_zcl_set_attr_value_param_t * p_savp)
{
    bulb_device_ctx_t* p_light_ctx = get_light_ctx_by_id(ep_id);

    zb_ret_t ret = RET_NOT_IMPLEMENTED;

    if (p_savp->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
    {
        if (p_savp->attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        {
            // ON - OFF management
            uint8_t value = p_savp->values.data8;
            zb_light_set_state(ep_id, (zb_bool_t)value);

            ret = RET_OK;
        }
    }
    else if (p_savp->cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
    {
        if (p_savp->attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
        {
            // Level management
            uint16_t value = p_savp->values.data16;
            zb_light_set_level(ep_id, value);

            ret = RET_OK;
        }
    }
    else
    {
        /* Other clusters can be processed here */
        NRF_LOG_INFO("Unhandled cluster attribute id: %d", p_savp->cluster_id);
    }

    return ret;
}

/** Sets light at given level and save it in the attributes */
zb_ret_t zb_light_set_level(uint8_t ep_id,
                            zb_uint8_t value)
{
    bulb_device_ctx_t* p_light_ctx = get_light_ctx_by_id(ep_id);

    ret_code_t err_code = NRF_SUCCESS;
    ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,                                       
                         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,            
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                 
                         ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 
                         (zb_uint8_t *)&value,                                       
                         ZB_FALSE);

    if (value == 0) {
        zb_uint8_t value_onoff = ZB_FALSE;
        ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
                             ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ZB_ZCL_CLUSTER_SERVER_ROLE,
                             ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                             &value_onoff,
                             ZB_FALSE);
    }
    else {
        zb_uint8_t value_onoff = ZB_TRUE;
        ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
                             ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ZB_ZCL_CLUSTER_SERVER_ROLE,
                             ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                             &value_onoff,
                             ZB_FALSE);
    }

    zb_set_brightness(value, p_light_ctx->ep_id);

    return (err_code == NRF_SUCCESS ? RET_OK : RET_ERROR);
}

/** Sets light at given state and save it in the attribute */
void zb_light_set_state(uint8_t ep_id, zb_bool_t value) {
    bulb_device_ctx_t* p_light_ctx = get_light_ctx_by_id(ep_id);

    ZB_ZCL_SET_ATTRIBUTE(p_light_ctx->ep_id,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&value,
                         ZB_FALSE);
    
    if (value) {   
        zb_light_set_level(p_light_ctx->ep_id, p_light_ctx->level_control_attr.current_level);
    }
    else {
        zb_set_brightness(0, p_light_ctx->ep_id);
    }
}
