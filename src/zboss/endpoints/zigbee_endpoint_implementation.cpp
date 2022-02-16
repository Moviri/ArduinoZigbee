#include <Scheduler.h>
#include "zigbee_endpoint_implementation.h"

static const char kZigbeeManufacturerName[] = "Arduino";
static const char kZigbeeDate[] = "20210911";

uint8_t ZigbeeEndpointImplementation::m_ep_id_counter = 0;

ZigbeeEndpointImplementation::ZigbeeEndpointImplementation(char model_id[], unsigned int power_source_type) : m_endpoint_id(++m_ep_id_counter)
{
    memset(&m_zboss_basic_data, 0, sizeof(m_zboss_basic_data));
    /* Basic cluster attributes data */
    m_zboss_basic_data.basic_attr.zcl_version = ZB_ZCL_VERSION;

    /* Identify cluster attributes data */
    m_zboss_basic_data.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* Basic cluster attributes data */
    m_zboss_basic_data.basic_attr.app_version = 1;
    m_zboss_basic_data.basic_attr.stack_version = 10;
    m_zboss_basic_data.basic_attr.hw_version = 11;

    ZB_ZCL_SET_STRING_VAL(m_zboss_basic_data.basic_attr.mf_name,
                          kZigbeeManufacturerName,
                          ZB_ZCL_STRING_CONST_SIZE(kZigbeeManufacturerName));

    ZB_ZCL_SET_STRING_VAL(m_zboss_basic_data.basic_attr.date_code,
                          kZigbeeDate,
                          ZB_ZCL_STRING_CONST_SIZE(kZigbeeDate));

    m_zboss_basic_data.basic_attr.power_source = power_source_type;

    /* Basic cluster attributes data
     * Note: the string is copied in a ZB_STRING that is a char array where the first byte is used to store the length and the "\0"
     * string terminator is not copied. */
    ZB_ZCL_SET_STRING_VAL(m_zboss_basic_data.basic_attr.model_id,
                          model_id,
                          MIN(strlen(model_id), sizeof(m_zboss_basic_data.basic_attr.model_id) - 1));

    /** Description of the physical location of the device (16 bytes). You can modify it during the commissioning process. */
#if 0
    // TODO: NOT WORKING
    ZB_ZCL_SET_STRING_VAL(m_zboss_basic_data.basic_attr.location_id,
                          location,
                          MIN(strlen(location), sizeof(m_zboss_basic_data.basic_attr.location_id) - 1));
#else
    static const char kZigbeeLocation[] = "Generic";
    ZB_ZCL_SET_STRING_VAL(m_zboss_basic_data.basic_attr.location_id,
                          kZigbeeLocation,
                          ZB_ZCL_STRING_CONST_SIZE(kZigbeeLocation));

#endif
    m_zboss_basic_data.basic_attr.ph_env = ZB_ZCL_BASIC_PHYSICAL_ENVIRONMENT_DEFAULT_VALUE;

    m_zboss_basic_data.cluster_revision_identify_attr_list = 1;
    zb_zcl_attr_t identify_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_basic_data.cluster_revision_identify_attr_list)},
        {ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, 0x21U, (0x01U | 0x02U), (void *)(&m_zboss_basic_data.identify_attr.identify_time)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_basic_data.identify_attr_list) == sizeof(identify_attr_list), "Check vector size");
    memcpy(m_zboss_basic_data.identify_attr_list, identify_attr_list, sizeof(identify_attr_list));

    m_zboss_basic_data.device_enable_basic_attr_list = 1U;
    m_zboss_basic_data.cluster_revision_basic_attr_list = 1;
    zb_zcl_attr_t basic_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_basic_data.cluster_revision_basic_attr_list)},
        {ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 0x20U, 0x01U, (void *)(&m_zboss_basic_data.basic_attr.zcl_version)},
        {ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, 0x20U, 0x01U, (void *)(&m_zboss_basic_data.basic_attr.app_version)},
        {ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, 0x20U, 0x01U, (void *)(&m_zboss_basic_data.basic_attr.stack_version)},
        {ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, 0x20U, 0x01U, (void *)(&m_zboss_basic_data.basic_attr.hw_version)},
        {ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, 0x42U, 0x01U, (void *)(m_zboss_basic_data.basic_attr.mf_name)},
        {ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, 0x42U, 0x01U, (void *)(m_zboss_basic_data.basic_attr.model_id)},
        {ZB_ZCL_ATTR_BASIC_DATE_CODE_ID, 0x42U, 0x01U, (void *)(m_zboss_basic_data.basic_attr.date_code)},
        {ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 0x30U, 0x01U, (void *)(&m_zboss_basic_data.basic_attr.power_source)},
        {ZB_ZCL_ATTR_BASIC_SW_BUILD_ID, 0x42U, 0x01U, (void *)(m_zboss_basic_data.basic_attr.sw_ver)},
        {ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID, 0x10U, (0x01U | 0x02U), (void *)&(m_zboss_basic_data.device_enable_basic_attr_list)},
        {ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID, 0x42U, (0x01U | 0x02U), (void *)(m_zboss_basic_data.basic_attr.location_id)},
        {ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID, 0x30U, (0x01U | 0x02U), (void *)(&m_zboss_basic_data.basic_attr.ph_env)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_basic_data.basic_attr_list) == sizeof(basic_attr_list), "Check vector size");
    memcpy(m_zboss_basic_data.basic_attr_list, basic_attr_list, sizeof(basic_attr_list));
}

void ZigbeeEndpointImplementation::update()
{
    /* Nothing to do. */
}

bool ZigbeeEndpointImplementation::setDescriptor(zb_af_endpoint_desc_t *descriptor)
{
    m_endpoint_descriptor = descriptor;
    return 1;
}

zb_af_endpoint_desc_t *ZigbeeEndpointImplementation::endpointDescriptor() const
{
    return m_endpoint_descriptor;
}

zb_uint8_t ZigbeeEndpointImplementation::processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params)
{
    return ZB_FALSE;
}

zb_ret_t ZigbeeEndpointImplementation::processCommandDV(zb_zcl_device_callback_param_t *cmd_params)
{
    return RET_ERROR;
}

void ZigbeeEndpointImplementation::feedbackEffect(zb_zcl_identify_effect_value_param_t *idt_params)
{
    /** Default behaviour. Can be overridden by child classes **/
    const uint32_t start_time = millis();
    switch (idt_params->effect_id)
    {
    case ZB_ZCL_IDENTIFY_EFFECT_ID_BLINK:
        /* Blink one time */
        analogWrite(LED_BUILTIN, 255);
        while (millis() - start_time < 2000)
        {
            yield();
        }
        analogWrite(LED_BUILTIN, 0);
        break;

    case ZB_ZCL_IDENTIFY_EFFECT_ID_OKAY:
        analogWrite(LEDG, 0);
        while (millis() - start_time < 1000)
        {
            yield();
        }
        analogWrite(LEDG, 255);
        break;

    default:
        break;
    }
}

void ZigbeeEndpointImplementation::setModelID(char model_id[])
{
    /* Basic cluster attributes data
     * Note: the string is copied in a ZB_STRING that is a char array where the first byte is used to store the length and the "\0"
     * string terminator is not copied. */
    ZB_ZCL_SET_STRING_VAL(m_zboss_basic_data.basic_attr.model_id,
                          model_id,
                          MIN(strlen(model_id), sizeof(m_zboss_basic_data.basic_attr.model_id) - 1));
}

uint8_t ZigbeeEndpointImplementation::endpointId() const
{
    return m_endpoint_id;
}