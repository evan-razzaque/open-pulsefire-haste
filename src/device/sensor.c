/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#include <stdint.h>
#include <hidapi/hidapi.h>

#include "sensor.h"
#include "mouse.h"

int set_polling_rate(hid_device *dev, byte polling_rate_value) {
	byte data[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_POLLING_RATE), 0x00, 0x00, 0x01, polling_rate_value};
	return mouse_write(dev, data);
}

/**
 * @brief Sets the lift off distance for the mouse.
 * 
 * @param dev The mouse device handle
 * @param lift_off_distance A LIFT_OFF_DISTANCE value
 * @return the number of bytes written or -1 on error
 */
static int set_lift_off_distance(hid_device *dev, LIFT_OFF_DISTANCE lift_off_distance) {
    byte data[PACKET_SIZE] = {
        REPORT_FIRST_BYTE(SEND_BYTE_DPI),
        SENSOR_CONFIG_BYTE_LIFT_OFF_DISTANCE,
        0x00,
        0x01,
        lift_off_distance,
        lift_off_distance
    };

    return mouse_write(dev, data);
}

int save_dpi_settings(hid_device *dev, dpi_settings *settings, byte lift_off_distance) {
    int res;
    
    byte data_enabled_profiles[PACKET_SIZE] = {
        REPORT_FIRST_BYTE(SEND_BYTE_DPI), 
        SENSOR_CONFIG_BYTE_ENABLED_DPI_PROFILES,
        0x00,
        0x01,
        settings->enabled_profile_bit_mask
    };

    res = mouse_write(dev, data_enabled_profiles);
    if (res < 0) return res;

    for (int i = 0; i < settings->profile_count; i++) {
        byte data_dpi_value[PACKET_SIZE] = {
            REPORT_FIRST_BYTE(SEND_BYTE_DPI),
            SENSOR_CONFIG_BYTE_DPI_VALUE,
            i,
            0x02,
            settings->profiles[i].dpi_value / 100
        };
        
        res = mouse_write(dev, data_dpi_value);
        if (res < 0) return res;

        byte data_dpi_color_indicator[PACKET_SIZE] = {
            REPORT_FIRST_BYTE(SEND_BYTE_DPI),
            SENSOR_CONFIG_BYTE_DPI_INDICATOR_COLOR,
            i,
            0x03,
            settings->profiles[i].indicator.red,
            settings->profiles[i].indicator.green,
            settings->profiles[i].indicator.blue
        };

        res = mouse_write(dev, data_dpi_color_indicator);
        if (res < 0) return res;
    }
    
    res = set_lift_off_distance(dev, lift_off_distance);
    if (res < 0) return res;

    byte data_select_profile[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_DPI), SENSOR_CONFIG_BYTE_SELECTED_DPI_PROFILE, 0x00, 0x01, settings->selected_profile};
    res = mouse_write(dev, data_select_profile);
    if (res < 0) return res;

    byte data_save_sensor_settings[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_SAVE_SETTINGS), SAVE_BYTE_DPI_PROFILE_INDICATORS};
    res = mouse_write(dev, data_save_sensor_settings);

    return res;
}
