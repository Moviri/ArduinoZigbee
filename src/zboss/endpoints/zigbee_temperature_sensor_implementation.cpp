#include <mbed.h>
#include "zigbee_temperature_sensor_implementation.h"
#include "../../endpoints/zigbee_temperature_sensor.h"

#define interface() static_cast<ZigbeeTemperatureSensor *>(m_interface)

const zb_uint8_t MIN_INTERVAL_BIT_SHIFT = 4 * sizeof(zb_uint8_t);
const zb_uint8_t MAX_INTERVAL_BIT_SHIFT = 6 * sizeof(zb_uint8_t);
const zb_uint32_t DEFAULT_TEMPERATURE_MEASUREMENT_PERIOD = 30000;

ZigbeeTemperatureSensorImplementation::ZigbeeTemperatureSensorImplementation(ZigbeeTemperatureSensor *interface, const zb_char_t model_id[], unsigned int power_source_type) : ZigbeeEndpointImplementation(interface, model_id, power_source_type)
{
    /* WARNING: do not use the interface object inside this constructor because it is not fully constructed. */
    memset(&m_zboss_data, 0, sizeof(m_zboss_data));

    /* Temperature measurement cluster attributes data */
    m_zboss_data.temp_attr.measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_UNKNOWN;
    m_zboss_data.temp_attr.min_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_MIN_VALUE;
    m_zboss_data.temp_attr.max_measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_MAX_VALUE;
    m_zboss_data.temp_attr.tolerance = ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_MAX_VALUE;

    m_zboss_data.cluster_revision_temperature_attr_list = 1;
    zb_zcl_attr_t temperature_attr_list[] = {
        {0xfffdU, 0x21U, 0x01U, (void *)&(m_zboss_data.cluster_revision_temperature_attr_list)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, 0x29U, 0x01U | 0x04U, (void *)(&m_zboss_data.temp_attr.measure_value)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, 0x29U, 0x01U, (void *)(&m_zboss_data.temp_attr.min_measure_value)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, 0x29U, 0x01U, (void *)(&m_zboss_data.temp_attr.max_measure_value)},
        {ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID, 0x21U, 0x01U, (void *)(&m_zboss_data.temp_attr.tolerance)},
        {(zb_uint16_t)(-1), 0, 0, __null}};
    static_assert(sizeof(m_zboss_data.temperature_attr_list) == sizeof(temperature_attr_list), "Check vector size");
    memcpy(m_zboss_data.temperature_attr_list, temperature_attr_list, sizeof(temperature_attr_list));

    zb_zcl_cluster_desc_t temperature_sensor_clusters[] = {
        {(0x0003U), ((sizeof(m_zboss_basic_data.identify_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_basic_data.identify_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_identify_init_server : (((0x01U) == 0x02U) ? zb_zcl_identify_init_client : __null))},
        {(0x0000U), ((sizeof(m_zboss_basic_data.basic_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_basic_data.basic_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_basic_init_server : (((0x01U) == 0x02U) ? zb_zcl_basic_init_client : __null))},
        {(0x0402U), ((sizeof(m_zboss_data.temperature_attr_list) / sizeof(zb_zcl_attr_t))), ((m_zboss_data.temperature_attr_list)), (0x01U), (0x0000), (((0x01U) == 0x01U) ? zb_zcl_temp_measurement_init_server : (((0x01U) == 0x02U) ? zb_zcl_temp_measurement_init_client : __null))},
        {(0x0003U), (0), (__null), (0x02U), (0x0000), (((0x02U) == 0x01U) ? zb_zcl_identify_init_server : (((0x02U) == 0x02U) ? zb_zcl_identify_init_client : __null))}};
    static_assert(sizeof(m_zboss_data.temperature_sensor_clusters) == sizeof(temperature_sensor_clusters), "Check vector size");
    memcpy(m_zboss_data.temperature_sensor_clusters, temperature_sensor_clusters, sizeof(temperature_sensor_clusters));

    m_zboss_data.simple_desc_temperature_sensor = {
        m_endpoint_id,
        0x0104U,
        ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID,
        0,
        0,
        3,
        1,
        {0x0000U, 0x0003U, 0x0402U, 0x0003U}};
    m_zboss_data.temperature_sensor = {
        m_endpoint_id,
        0x0104U,
        __null,
        __null,
        0,
        (void *)__null,
        (sizeof(m_zboss_data.temperature_sensor_clusters) / sizeof(zb_zcl_cluster_desc_t)),
        m_zboss_data.temperature_sensor_clusters,
        (zb_af_simple_desc_1_1_t *)&m_zboss_data.simple_desc_temperature_sensor,
        (1),
        m_zboss_data.reporting_infotemperature_sensor,
        0,
        __null};

    setDescriptor(&m_zboss_data.temperature_sensor);
};

zb_uint8_t ZigbeeTemperatureSensorImplementation::processCommandEP(zb_bufid_t bufid, zb_zcl_parsed_hdr_t *cmd_params)
{
    if (cmd_params->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV &&
        cmd_params->is_common_command &&
        cmd_params->cluster_id == ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT)
    {
        switch (cmd_params->cmd_id)
        {
        case ZB_ZCL_CMD_CONFIG_REPORT:
        {
            const zb_uint8_t *p_payload = (const zb_uint8_t *)zb_buf_begin(bufid);

            /* The architecture is LE so we can directly read the buffer */
            zb_uint16_t min_interval = *(const zb_uint16_t *)(p_payload + MIN_INTERVAL_BIT_SHIFT);
            zb_uint16_t max_interval = *(const zb_uint16_t *)(p_payload + MAX_INTERVAL_BIT_SHIFT);

            m_interface->m_period = checkUpdatePeriod(min_interval, max_interval, DEFAULT_TEMPERATURE_MEASUREMENT_PERIOD);
            break;
        }
        case ZB_ZCL_CMD_READ_ATTRIB:
        {
            update();
            break;
        }
        default:
            break;
        }
    }

    return ZB_FALSE;
}

void ZigbeeTemperatureSensorImplementation::update()
{
    int16_t curr_temp_cast = (int16_t)(interface()->m_read_temperature() * 100.0);

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(m_endpoint_id,
                                                     ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE,
                                                     ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
                                                     (zb_uint8_t *)&curr_temp_cast,
                                                     ZB_FALSE);
}

void ZigbeeTemperatureSensorImplementation::begin(bool load_from_memory)
{
	m_started = true;
    if (!load_from_memory)
    {
        return;
    }
    // Read reporting period from Zboss dedicated memory that in this case is m_zboss_data.reporting_infotemperature_sensor[0]
    m_interface->m_period = zb_zcl_find_reporting_info(m_endpoint_id,
                                                       ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                                                       ZB_ZCL_CLUSTER_SERVER_ROLE,
                                                       ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID)
                                ->u.send_info.min_interval *
                            1000;
}

void ZigbeeTemperatureSensorImplementation::onLeave()
{
    /* Reset reporting times to avoid useless measurements */
    m_interface->m_period = UINT32_MAX;
}
