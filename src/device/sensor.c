#include <stdint.h>
#include <stdio.h>
#include <hidapi/hidapi.h>

#include "mouse.h"
#include "sensor.h"

int set_polling_rate(hid_device *dev, byte polling_rate_value) {
	byte data[PACKET_SIZE] = {SEND_BYTE_POLLING_RATE, 0x00, 0x00, 0x01, polling_rate_value};
	return mouse_write(dev, data);
}

static int set_lift_off_distance(hid_device *dev, byte lift_off_distance) {
    byte data[PACKET_SIZE] = {
        SEND_BYTE_DPI,
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
        SEND_BYTE_DPI, 
        SENSOR_CONFIG_BYTE_ENABLED_DPI_PROFILES,
        0x00,
        0x01,
        settings->enabled_profile_bit_mask
    };

    res = mouse_write(dev, data_enabled_profiles);

    for (int i = 0; i < settings->profile_count; i++) {
        byte data_dpi_value[PACKET_SIZE] = {
            SEND_BYTE_DPI,
            SENSOR_CONFIG_BYTE_DPI_VALUE,
            i,
            0x02,
            settings->profiles[i].dpi_value / 100
        };

        res = mouse_write(dev, data_dpi_value);

        byte data_dpi_color_indicator[PACKET_SIZE] = {
            SEND_BYTE_DPI,
            SENSOR_CONFIG_BYTE_DPI_INDICATOR_COLOR,
            i,
            0x03,
            settings->profiles[i].indicator.red,
            settings->profiles[i].indicator.green,
            settings->profiles[i].indicator.blue
        };

        res = mouse_write(dev, data_dpi_color_indicator);
    }

    res = set_lift_off_distance(dev, lift_off_distance);

    byte data_select_profile[PACKET_SIZE] = {SEND_BYTE_DPI, SENSOR_CONFIG_BYTE_SELECTED_DPI_PROFILE, 0x00, 0x01, settings->selected_profile};
    res = mouse_write(dev, data_select_profile);    

    byte data_save_sensor_settings[PACKET_SIZE] = {SEND_BYTE_SAVE_SETTINGS, SAVE_BYTE_DPI_PROFILE_INDICATORS};
    res = mouse_write(dev, data_save_sensor_settings);

    return res;
}
