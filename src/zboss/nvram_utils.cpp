#include "nvram_utils.h"

#include "KVStore.h"
#include "kvstore_global_api.h"
#include "mbed_error.h"

namespace zigbee_utils
{
    extern const unsigned int kBuildTimestamp;
}

static bool s_sketch_changed = false;

static const char kTimestampKey[] = "/kv/timestamp";

bool isSketchChanged()
{
    if (s_sketch_changed)
    {
        // Once we have detected that the sketch changed there is no need to check again till another restart.
        return true;
    }

    // Retrieve the previous timestamp
    uint32_t timestamp = 0;
    size_t actual_size = 0;
    int result = kv_get(kTimestampKey, &timestamp, sizeof(timestamp), &actual_size);

    switch (result)
    {
    case MBED_SUCCESS:
        if (timestamp == zigbee_utils::kBuildTimestamp)
        {
            return false;
        }
        break;
    case MBED_ERROR_ITEM_NOT_FOUND:
        break;
    default:
        // Error reading from key-value store.
        goto handle_error;
    }

    // Update the timestamp.
    s_sketch_changed = true;
    timestamp = zigbee_utils::kBuildTimestamp;
    result = kv_set(kTimestampKey, &timestamp, sizeof(timestamp), 0);
    if (result == MBED_SUCCESS)
    {
        return s_sketch_changed;
    }

handle_error:
    // Stop the sketch if an error occurs
    while (true)
        ;
    return s_sketch_changed;
}