#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "device/rgb.h"
#include "device/mouse.h"
#include "device/buttons.h"
#include "device/sensor.h"
#include "types.h"

#include "mouse_config.h"

#define SETTINGS_FILE "data/mouse_settings.bin"

static void create_settings_file(mouse_settings *settings, app_data *data) {
    *settings = (mouse_settings) {
        .led = {.red = 0xff, .brightness = 100},
        .bindings = {
            LEFT_CLICK,
            RIGHT_CLICK,
            MIDDLE_CLICK,
            BACK,
            FORWARD,
            DPI_TOGGLE
        },
        .dpi_config = {
            .enabled_profile_bit_mask = 0b00001,
            .profiles = {
                {.dpi_value = 1000, .indicator = {.red = 0xff}}
            },
            .profile_count = 1,
            .selected_profile = 0
        },
        .polling_rate_value = 3,
        .lift_off_distance = 2
    };
    
    data->settings_file = fopen(SETTINGS_FILE, "wb");
    fwrite(settings, sizeof(mouse_settings), 1, data->settings_file);
    fflush(data->settings_file);
}

void load_settings_from_file(app_data *data) {
    mouse_settings settings = {0};
    data->settings_file = fopen(SETTINGS_FILE, "rb");

    if (data->settings_file == NULL) {
        printf("new file\n");
        create_settings_file(&settings, data);
    } else {
        fread(&settings, sizeof(mouse_settings), 1, data->settings_file);
        fclose(data->settings_file);
        data->settings_file = fopen(SETTINGS_FILE, "wb");
    }

    memcpy(data->button_data.bindings, settings.bindings, sizeof(settings.bindings));
    
    data->color_data.mouse_led = settings.led;
    data->sensor_data.dpi_config = settings.dpi_config;
    data->sensor_data.polling_rate_value = settings.polling_rate_value;
    data->sensor_data.lift_off_distance = settings.lift_off_distance;
}

void save_settings_to_file(app_data *data) {
    mouse_settings settings = {0};

    memcpy(settings.bindings, data->button_data.bindings, sizeof(data->button_data.bindings));
    
    settings.led = data->color_data.mouse_led;
    settings.dpi_config = data->sensor_data.dpi_config;
    settings.polling_rate_value = data->sensor_data.polling_rate_value;
    settings.lift_off_distance = data->sensor_data.lift_off_distance;

    rewind(data->settings_file);
    fwrite(&settings, sizeof(mouse_settings), 1, data->settings_file);
    fclose(data->settings_file);
}
