#include "endpoint_ctx.h"

EndpointCTX::EndpointCTX(float period_p) : period{period_p}
{
    ep_id = ++ep_id_counter;

    /* Basic cluster attributes data */
    basic_attr.zcl_version = ZB_ZCL_VERSION;

    /* Identify cluster attributes data */
    identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    m_zboss_data.cluster_revision_identify_attr_list = 1;
    zb_zcl_attr_t identify_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_identify_attr_list)},
        {ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, 0x21U, (0x01U | 0x02U), (void *)(&identify_attr.identify_time)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_data.identify_attr_list) == sizeof(identify_attr_list), "Check vector size");
    memcpy(m_zboss_data.identify_attr_list, identify_attr_list, sizeof(identify_attr_list));

    m_zboss_data.device_enable_basic_attr_list = 1U;
    m_zboss_data.cluster_revision_basic_attr_list = 1;
    zb_zcl_attr_t basic_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_basic_attr_list)},
        {ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 0x20U, 0x01U, (void *)(&basic_attr.zcl_version)},
        {ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, 0x20U, 0x01U, (void *)(&basic_attr.app_version)},
        {ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, 0x20U, 0x01U, (void *)(&basic_attr.stack_version)},
        {ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, 0x20U, 0x01U, (void *)(&basic_attr.hw_version)},
        {ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, 0x42U, 0x01U, (void *)(basic_attr.mf_name)},
        {ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, 0x42U, 0x01U, (void *)(basic_attr.model_id)},
        {ZB_ZCL_ATTR_BASIC_DATE_CODE_ID, 0x42U, 0x01U, (void *)(basic_attr.date_code)},
        {ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 0x30U, 0x01U, (void *)(&basic_attr.power_source)},
        {ZB_ZCL_ATTR_BASIC_SW_BUILD_ID, 0x42U, 0x01U, (void *)(basic_attr.sw_ver)},
        {ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID, 0x10U, (0x01U | 0x02U), (void *)&(m_zboss_data.device_enable_basic_attr_list)},
        {ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID, 0x42U, (0x01U | 0x02U), (void *)(basic_attr.location_id)},
        {ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID, 0x30U, (0x01U | 0x02U), (void *)(&basic_attr.ph_env)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_data.basic_attr_list) == sizeof(basic_attr_list), "Check vector size");
    memcpy(m_zboss_data.basic_attr_list, basic_attr_list, sizeof(basic_attr_list));
}

bool EndpointCTX::set_desc(zb_af_endpoint_desc_t *ep_desc_p)
{
    ep_desc = ep_desc_p;
    return 1;
}

void EndpointCTX::setAttribute(zb_zcl_set_attr_value_param_t *attr_p)
{
}

void EndpointCTX::setLevelControl(zb_zcl_level_control_set_value_param_t level_p)
{
}
