#include "endpoint_ctx.h"

EndpointCTX::EndpointCTX(float period_p) : period{period_p}
{
    ep_id = ++ep_id_counter;

    /* Basic cluster attributes data */
    basic_attr.zcl_version = ZB_ZCL_VERSION;

    /* Identify cluster attributes data */
    identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
}

bool EndpointCTX::set_desc(zb_af_endpoint_desc_t *ep_desc_p) // Setter function for ep_desc attribute
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
