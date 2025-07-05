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
#define MACROS_FILE "data/macros.bin"

/**
 * @brief A struct used for reading and writing mouse settings with the disk.
 */
struct mouse_settings {
	color_options led;

	uint16_t bindings[6];

	dpi_settings dpi_config;
	byte polling_rate_value;
	byte lift_off_distance;
} typedef mouse_settings;

struct mouse_macro_info {
    int macro_count;
    int macro_indicies[6]; // Contains a macro index for each mouse button. Non-macro bindings are -1.
} typedef mouse_macro_info;

struct macro_detail {
    size_t macro_name_length;
    int event_count;
} typedef macro_detail;

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
    fclose(data->settings_file);
}

void load_settings_from_file(app_data *data) {
    mouse_settings settings = {0};
    data->settings_file = fopen(SETTINGS_FILE, "rb");

    if (data->settings_file == NULL) {
        create_settings_file(&settings, data);
    } else {
        fread(&settings, sizeof(mouse_settings), 1, data->settings_file);
        fclose(data->settings_file);
    }

    memcpy(data->button_data.bindings, settings.bindings, sizeof(settings.bindings));
    
    data->color_data.mouse_led = settings.led;
    data->sensor_data.dpi_config = settings.dpi_config;
    data->sensor_data.polling_rate_value = settings.polling_rate_value;
    data->sensor_data.lift_off_distance = settings.lift_off_distance;
}

void save_settings_to_file(app_data *data) {
    mouse_settings settings = {
        .led = data->color_data.mouse_led,
        .dpi_config = data->sensor_data.dpi_config,
        .polling_rate_value = data->sensor_data.polling_rate_value,
        .lift_off_distance = data->sensor_data.lift_off_distance
    };

    memcpy(settings.bindings, data->button_data.bindings, sizeof(data->button_data.bindings));
    
    data->settings_file = fopen(SETTINGS_FILE, "wb");
    fwrite(&settings, sizeof(mouse_settings), 1, data->settings_file);
    fclose(data->settings_file);
}

void load_macros_from_file(app_data *data) {
    mouse_macro_info macro_info = {.macro_count = 0, .macro_indicies = {-1, -1, -1, -1, -1, -1}};
    
    data->macros_file = fopen(MACROS_FILE, "rb");

    if (data->macros_file == NULL) {
        data->macros_file = fopen(MACROS_FILE, "wb");
        fwrite(&macro_info, sizeof(mouse_macro_info), 1, data->macros_file);
        fclose(data->macros_file);
    } else {
        fread(&macro_info, sizeof(mouse_macro_info), 1, data->macros_file);
    }

    memcpy(data->macro_data.macro_indicies, macro_info.macro_indicies, sizeof(macro_info.macro_indicies));

    if (macro_info.macro_count == 0) return;
    
    data->macro_data.macro_count = macro_info.macro_count;
    data->macro_data.macro_array_size = macro_info.macro_count;
    data->macro_data.macros = malloc(sizeof(mouse_macro) * macro_info.macro_count);

    mouse_macro *macros = data->macro_data.macros;

    for (int i = 0; i < macro_info.macro_count; i++) {
        macro_detail detail = {0};
        fread(&detail, sizeof(macro_detail), 1, data->macros_file);

        macros[i].name = malloc(sizeof(char) * detail.macro_name_length);
        fread(macros[i].name, sizeof(char), detail.macro_name_length, data->macros_file);

        macros[i].generic_event_count = detail.event_count;
        macros[i].generic_event_array_size = detail.event_count;
        macros[i].events = malloc(sizeof(generic_macro_event) * detail.event_count);
        fread(macros[i].events, sizeof(generic_macro_event), detail.event_count, data->macros_file);
    }

    fclose(data->macros_file);
}

void save_macros_to_file(app_data *data) {
    mouse_macro_info macro_info = {
        .macro_count = data->macro_data.macro_count,
    };

    memcpy(macro_info.macro_indicies, data->macro_data.macro_indicies, sizeof(int) * BUTTON_COUNT);

    data->macros_file = fopen(MACROS_FILE, "wb");
    fwrite(&macro_info, sizeof(mouse_macro_info), 1, data->macros_file);

    mouse_macro *macros = data->macro_data.macros;
    
    for (int i = 0; i < macro_info.macro_count; i++) {
        macro_detail detail = {
            .macro_name_length = strlen(macros[i].name) + 1,
            .event_count = macros[i].generic_event_count
        };

        fwrite(&detail, sizeof(macro_detail), 1, data->macros_file);
        fwrite(macros[i].name, sizeof(char), detail.macro_name_length, data->macros_file);
        fwrite(macros[i].events, sizeof(generic_macro_event), detail.event_count, data->macros_file);
    }

    fclose(data->macros_file);
}
