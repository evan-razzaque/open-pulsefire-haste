#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <hidapi/hidapi.h>

#include "mouse.h"
#include "rgb.h"

/**
 * @brief An enum for mode bytes for dpi settings and lift-off distance.
 * Members are ordered by the order in which their corresponding packets are sent.
 */
typedef enum SENSOR_CONFIG_MODE_BYTE {
    SENSOR_CONFIG_BYTE_ENABLED_DPI_PROFILES = 0x01,
    SENSOR_CONFIG_BYTE_DPI_VALUE            = 0x02,
    SENSOR_CONFIG_BYTE_DPI_INDICATOR_COLOR  = 0x03,
    SENSOR_CONFIG_BYTE_LIFT_OFF_DISTANCE    = 0x05,
    SENSOR_CONFIG_BYTE_SELECTED_DPI_PROFILE = 0x00,
} SENSOR_CONFIG_MODE_BYTE;

/**
 * @brief A struct for a dpi profile.
 */
struct dpi_profile {
	uint16_t dpi_value;
    /** The color that is briefly shown when switching to this profile */
	color_options indicator;
} typedef dpi_profile;

/**
 * @brief A struct for storing dpi settings.
 */
struct dpi_settings {
    dpi_profile profiles[5];
    byte profile_count;
    byte selected_profile;
    /** A 5 bit number where each bit enables/disables at its position */
    byte enabled_profile_bit_mask;
} typedef dpi_settings;

/**
 * @brief Sets the polling rate for the mouse.
 * 
 * @param dev The mouse device handle
 * @param polling_rate_value The polling rate value
 * @return the number of bytes written or -1 on error
 */
int set_polling_rate(hid_device *dev, byte polling_rate_value);

/**
 * @brief Saves dpi settings to mouse's on-board memory. Also sets and saves lift-off distance, because
 * there's literally no other way to do it.
 * 
 * @param dev The mouse device handle
 * @param profiles The dpi settings for the mouse
 * @param lift_off_distance The lift-off distance for the mouse
 * @return the number of bytes written or -1 on error
 */
int save_dpi_settings(hid_device *dev, dpi_settings *settings, byte lift_off_distance);

#endif
