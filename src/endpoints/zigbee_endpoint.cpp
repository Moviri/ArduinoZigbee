#include "zigbee_endpoint.h"
#include "../zboss/endpoints/zigbee_endpoint_implementation.h"

ZigbeeEndpoint::ZigbeeEndpoint(ZigbeeEndpointImplementation *impl) : m_endpoint_id(impl->endpointId()),
                                                                     m_period(UINT32_MAX),
                                                                     m_last_update_time(0),
                                                                     m_impl(impl)
{
}

ZigbeeEndpoint::~ZigbeeEndpoint() {}

void ZigbeeEndpoint::update()
{
    /* Nothing to do. */
}

uint8_t ZigbeeEndpoint::endpointId() const
{
    return m_endpoint_id;
}

uint32_t ZigbeeEndpoint::period() const
{
    return m_period;
}

uint32_t ZigbeeEndpoint::lastUpdateTime() const
{
    return m_last_update_time;
}

ZigbeeEndpointImplementation *ZigbeeEndpoint::implementation() const
{
    return m_impl.get();
}