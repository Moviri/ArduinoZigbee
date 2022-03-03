#include "nvram_utils.h"

#include "Arduino.h"
#include <FlashIAP.h>
#include <FlashIAPBlockDevice.h>
#include <TDBStore.h>

namespace zigbee_utils
{
    extern const unsigned int kBuildTimestamp;
}

// A helper struct for FlashIAP limits
struct FlashIAPLimits
{
    size_t flash_size;
    uint32_t start_address;
    uint32_t available_size;
};

static FlashIAPLimits getFlashIAPLimits();

// Get limits of the In Application Program (IAP) flash, ie. the internal MCU flash.
auto iapLimits{getFlashIAPLimits()};

// Create a block device on the available space of the FlashIAP
FlashIAPBlockDevice blockDevice(iapLimits.start_address, iapLimits.available_size);

// Create a key-value store on the Flash IAP block device
mbed::TDBStore store(&blockDevice);

static bool s_sketch_changed = false;

static const char kTimestampKey[] = "build_timestamp";

// Get the actual start address and available size for the FlashIAP Block Device
// considering the space already occupied by the sketch (firmware).
static FlashIAPLimits getFlashIAPLimits()
{
    // Alignment lambdas
    auto align_down = [](uint64_t val, uint64_t size)
    {
        return (((val) / size)) * size;
    };
    auto align_up = [](uint32_t val, uint32_t size)
    {
        return (((val - 1) / size) + 1) * size;
    };

    size_t flash_size;
    uint32_t flash_start_address;
    uint32_t start_address;
    mbed::FlashIAP flash;

    auto result = flash.init();
    if (result != 0)
    {
        return {};
    }

    // Find the start of first sector after text area
    int sector_size = flash.get_sector_size(FLASHIAP_APP_ROM_END_ADDR);
    start_address = align_up(FLASHIAP_APP_ROM_END_ADDR, sector_size);
    flash_start_address = flash.get_flash_start();
    flash_size = flash.get_flash_size();

    result = flash.deinit();

    int available_size = flash_start_address + flash_size - start_address;
    if (available_size % (sector_size * 2))
    {
        available_size = align_down(available_size, sector_size * 2);
    }

    return {flash_size, start_address, available_size};
}

/**
 * @brief Retrieve the timestamp from the the key-value store
 *
 * @param key
 * @param timestamp
 * @return int
 */
static int getSketchTimestamp(const char *key, uint32_t *timestamp)
{
    // Retrieve key-value info
    mbed::TDBStore::info_t info;
    auto result = store.get_info(key, &info);

    if (result == MBED_ERROR_ITEM_NOT_FOUND)
    {
        return result;
    }

    // Allocate space for the value
    uint8_t buffer[info.size]{};
    size_t actual_size;

    // Get the value
    result = store.get(key, buffer, sizeof(buffer), &actual_size);
    if (result != MBED_SUCCESS)
    {
        return result;
    }

    memcpy(timestamp, buffer, sizeof(uint32_t));
    return result;
}

/**
 * @brief Store a timestamp to the the key-value store
 *
 * @param key
 * @param timestamp
 * @return int
 */
static int setSketchTimestamp(const char *key, uint32_t timestamp)
{
    return store.set(key, reinterpret_cast<uint8_t *>(&stats), sizeof(uint32_t), 0);
}

bool isSketchChanged()
{
    if (s_sketch_changed)
    {
        // Once we have detected that the sketch changed there is no need to check again till another restart.
        return true;
    }
    auto result = store.init();
    // Stop the sketch if an error occurs
    if (result != MBED_SUCCESS)
    {
        while (true)
            ;
    }

    // Retrieve the previous timestamp
    uint32_t previous_timestamp = 0;
    result = getSketchTimestamp(kTimestampKey, &previous_timestamp);

    switch (result)
    {
    case MBED_SUCCESS:
        if (previous_timestamp != zigbee_utils::kBuildTimestamp)
        {
            s_sketch_changed = true;
            goto update_timestamp;
        }
        break;
    case MBED_ERROR_ITEM_NOT_FOUND:
        s_sketch_changed = true;
        goto update_timestamp;
        break;
    default:
        // Error reading from key-value store.
        while (true)
            ;
        break;
    }
    return s_sketch_changed;

update_timpestamp:
    result = setSketchTimestamp(kTimestampKey, zigbee_utils::kBuildTimestamp);
    // Stop the sketch if an error occurs
    if (result != MBED_SUCCESS)
    {
        while (true)
            ;
    }
    return s_sketch_changed;
}