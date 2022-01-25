#include <mbed.h>
#include "zigbee_temperature_sensor.h"
#include "zboss_api_aps.h"


/* Max sizes */
#define MAX_EP_TEMPERATURE_SENSOR 4

/* Basic cluster attributes initial values. For more information, see section 3.2.2.2 of the ZCL specification. */
#define SENSOR_INIT_BASIC_APP_VERSION       01                                  /**< Version of the application software (1 byte). */
#define SENSOR_INIT_BASIC_STACK_VERSION     10                                  /**< Version of the implementation of the Zigbee stack (1 byte). */
#define SENSOR_INIT_BASIC_HW_VERSION        11                                  /**< Version of the hardware of the device (1 byte). */
#define SENSOR_INIT_BASIC_MANUF_NAME        "Arduino"                           /**< Manufacturer name (32 bytes). */
#define SENSOR_INIT_BASIC_MODEL_ID          "Temperature Sensor v1"             /**< Model number assigned by the manufacturer (32-bytes long string). */
#define SENSOR_INIT_BASIC_DATE_CODE         "20211111"                          /**< Date provided by the manufacturer of the device in ISO 8601 format (YYYYMMDD), for the first 8 bytes. The remaining 8 bytes are manufacturer-specific. */
#define SENSOR_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE /**< Type of power source or sources available for the device. For possible values, see section 3.2.2.2.8 of the ZCL specification. */
#define SENSOR_INIT_BASIC_LOCATION_DESC     "Generic"                           /**< Description of the physical location of the device (16 bytes). You can modify it during the commisioning process. */
#define SENSOR_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED        /**< Description of the type of physical environment. For possible values, see section 3.2.2.2.10 of the ZCL specification. */

temp_sensor_ctx_t *ctx_temp_arr[MAX_EP_TEMPERATURE_SENSOR];
int current_temp_size = 0;

uint32_t last_trig_time[MAX_EP_TEMPERATURE_SENSOR] = {};

temp_sensor_ctx_t* get_temp_ctx_by_id(uint8_t ep_id) {
    for (uint8_t i = 0; i < current_temp_size; i++)
    {
        if (ctx_temp_arr[i]->ep_id == ep_id) {
            return ctx_temp_arr[i];
        }
    }
}

bool initialize_temp_sensor_EP(temp_sensor_ctx_t* p_temp_ctx, uint8_t ep_id, float (*aTempCB)(), float period) {
    // Clusters initialization
    memset(p_temp_ctx, 0, sizeof(temp_sensor_ctx_t));
    p_temp_ctx->ep_id = ep_id;
    temp_clusters_attr_init(p_temp_ctx);

    // Callback
    p_temp_ctx->tempCB = aTempCB;
    p_temp_ctx->period = period;

    // Context registration
    ctx_temp_arr[current_temp_size] = p_temp_ctx;
    current_temp_size++;                                   // Update number of temperature sensors

    return 1;
}

void temp_sensor_periodic_CB(uint32_t curr_time) 
{
    for (uint8_t i = 0; i < current_temp_size; i++)
    {
        if (curr_time - last_trig_time[i] >= ctx_temp_arr[i]->period) {
            float curr_temp = ctx_temp_arr[i]->tempCB();
            curr_temp *= 100;
            int16_t curr_temp_cast = (int16_t) curr_temp;

            zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(ctx_temp_arr[i]->ep_id, 
                                                             ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
                                                             ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                             ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, 
                                                             (zb_uint8_t *)&curr_temp_cast, 
                                                             ZB_FALSE);
            last_trig_time[i] = curr_time;
            // Serial.print("New temp value for EP ");  // test only
            // Serial.print(ctx_temp_arr[i]->ep_id);
            // Serial.print(": ");
            // Serial.println(curr_temp_cast);
        }
    }
}

void temp_clusters_attr_init(temp_sensor_ctx_t* p_temp_ctx)
{
    /* Basic cluster attributes data */
    p_temp_ctx->basic_attr.zcl_version   = ZB_ZCL_VERSION;
    p_temp_ctx->basic_attr.app_version   = SENSOR_INIT_BASIC_APP_VERSION;
    p_temp_ctx->basic_attr.stack_version = SENSOR_INIT_BASIC_STACK_VERSION;
    p_temp_ctx->basic_attr.hw_version    = SENSOR_INIT_BASIC_HW_VERSION;

    ZB_ZCL_SET_STRING_VAL(p_temp_ctx->basic_attr.mf_name,
                          SENSOR_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(p_temp_ctx->basic_attr.model_id,
                          SENSOR_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(p_temp_ctx->basic_attr.date_code,
                          SENSOR_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_DATE_CODE));

    p_temp_ctx->basic_attr.power_source = SENSOR_INIT_BASIC_POWER_SOURCE;

    ZB_ZCL_SET_STRING_VAL(p_temp_ctx->basic_attr.location_id,
                          SENSOR_INIT_BASIC_LOCATION_DESC,
                          ZB_ZCL_STRING_CONST_SIZE(SENSOR_INIT_BASIC_LOCATION_DESC));

    p_temp_ctx->basic_attr.ph_env = SENSOR_INIT_BASIC_PH_ENV;

    /* Identify cluster attributes data */
    p_temp_ctx->identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* Temperature measurement cluster attributes data */
    p_temp_ctx->temp_attr.measure_value     = ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_UNKNOWN;
    p_temp_ctx->temp_attr.min_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_MIN_VALUE;
    p_temp_ctx->temp_attr.max_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_MAX_VALUE;
    p_temp_ctx->temp_attr.tolerance         = ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_MAX_VALUE;
}
