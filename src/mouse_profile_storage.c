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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <gtk/gtk.h>

#include "mouse_profile_storage.h"

#include "device/rgb.h"
#include "device/mouse.h"
#include "device/buttons.h"
#include "device/sensor.h"
#include "types.h"

#include "config_led.h"
#include "config_buttons.h"
#include "config_macro.h"
#include "config_sensor.h"
#include "util.h"

/**
 * @brief A struct used for reading and writing mouse settings with the disk.
 */
struct mouse_profile {
	color_options led;

	uint16_t bindings[6];

	dpi_settings dpi_config;
	byte polling_rate_value;
	byte lift_off_distance;

    struct {
        int macro_count;
        int macro_indicies[6]; // Contains a macro index for each mouse button. Non-macro bindings are -1.
    } macro_info;
} typedef mouse_profile;

/**
 * @brief A struct used for reading and writing information about a `recorded_macro` with the disk.
 * 
 */
struct macro_detail {
    size_t macro_name_length;
    int event_count;
    REPEAT_MODE repeat_mode;
} typedef macro_detail;

static int handle_file_error(FILE *file, const char* filename, bool file_should_exist) {
    int error = errno;
    
    if (error == 2 && !file_should_exist) return 0;

    if (file == NULL && error != 0) {
        printf("Error opening %s: %s\n", filename, strerror(error));
        return -1;
    }

    return 0;
}

int create_data_directory(app_data *data) {
    const char *user_data_dir = g_get_user_data_dir();
    size_t user_data_dir_len = strlen(user_data_dir) + 1;

    size_t app_data_dir_size = user_data_dir_len * sizeof(char) + sizeof(APP_DIR);
    data->app_data_dir = malloc(app_data_dir_size);
    data->app_data_dir_length = app_data_dir_size / sizeof(char);

    strncpy(data->app_data_dir, user_data_dir, user_data_dir_len);
    strncat(data->app_data_dir, APP_DIR, sizeof(APP_DIR) / sizeof(char));
    
    int res = g_mkdir_with_parents(data->app_data_dir, S_IRWXU);
    return res;
}

static char* application_data_get_file_path(app_data *data, char* filename) {
    int filename_length = strlen(filename) + 1;

    char *path = malloc((data->app_data_dir_length * sizeof(char)) + (filename_length * sizeof(char)));
    strncpy(path, data->app_data_dir, data->app_data_dir_length);
    strncat(path, filename, filename_length);

    return path;
}

/**
 * @brief Creates a mouse profile.
 * 
 * @param settings The mouse_profile object
 * @param profile_path The path to save the profile to
 * @param data Application wide data structure
 * @return 0 if the profile file was created or -1 if there was an error
 */
static int create_profile(mouse_profile *profile, char *profile_path, app_data *data) {
    *profile = (mouse_profile) {
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
                {.dpi_value = 400, .indicator = {.red = 0xff}}
            },
            .profile_count = 1,
            .selected_profile = 0
        },
        .polling_rate_value = 3,
        .lift_off_distance = 2,
        .macro_info.macro_count = 0,
        .macro_info.macro_indicies = {-1, -1, -1, -1, -1, -1}
    };
    
    data->profile_file = fopen(profile_path, "wb");

    int res = handle_file_error(data->profile_file, profile_path, true);
    if (res < 0) return res;

    res = fwrite(profile, sizeof(mouse_profile), 1, data->profile_file);
    if (res != 1) {
        debug("Error\n");
        res = -1;
        fclose(data->profile_file);
    }

    return res;
}

static void load_profile_settings(app_data *data, char *profile_path) {
    mouse_profile settings = {0};

    int res;

    if (data->profile_file == NULL) {
        res = create_profile(&settings, profile_path, data);
        if (res < 0) {
            debug("Error: %d\n",, res);
            return;
        };
    } else {
        res = fread(&settings, sizeof(mouse_profile), 1, data->profile_file);

        if (res != 1) {
            debug("Error: %d\n",, res);
            return;
        }
    }

    memcpy(data->button_data->bindings, settings.bindings, sizeof(settings.bindings));
    
    data->color_data->mouse_led = settings.led;
    data->sensor_data->dpi_config = settings.dpi_config;
    data->sensor_data->polling_rate_value = settings.polling_rate_value;
    data->sensor_data->lift_off_distance = settings.lift_off_distance;

    data->macro_data->macro_count = settings.macro_info.macro_count;
    memcpy(
        data->macro_data->macro_indicies,
        settings.macro_info.macro_indicies,
        sizeof(settings.macro_info.macro_indicies)
    );
}

static void load_profile_macros(app_data *data) {
    data->macro_data->macro_array_size = MAX(data->macro_data->macro_count, 1);
    data->macro_data->macros = malloc(sizeof(recorded_macro) * data->macro_data->macro_array_size);

    recorded_macro *macros = data->macro_data->macros;

    for (int i = 0; i < data->macro_data->macro_count; i++) {
        macro_detail detail = {0};
        fread(&detail, sizeof(macro_detail), 1, data->profile_file);

        macros[i].name = malloc(sizeof(char) * detail.macro_name_length);
        fread(macros[i].name, sizeof(char), detail.macro_name_length, data->profile_file);

        macros[i].generic_event_count = detail.event_count;
        macros[i].generic_event_array_size = detail.event_count;
        macros[i].events = malloc(sizeof(generic_macro_event) * detail.event_count);
        fread(macros[i].events, sizeof(generic_macro_event), detail.event_count, data->profile_file);

        macros[i].repeat_mode = detail.repeat_mode;
    }
}

static void save_profile_settings(app_data *data) {
    mouse_profile profile = {
        .led = data->color_data->mouse_led,
        .dpi_config = data->sensor_data->dpi_config,
        .polling_rate_value = data->sensor_data->polling_rate_value,
        .lift_off_distance = data->sensor_data->lift_off_distance,
        .macro_info.macro_count = data->macro_data->macro_count
    };

    memcpy(profile.bindings, data->button_data->bindings, sizeof(data->button_data->bindings));
    memcpy(
        profile.macro_info.macro_indicies,
        data->macro_data->macro_indicies,
        sizeof(profile.macro_info.macro_indicies)
    );
    
    int res = fwrite(&profile, sizeof(mouse_profile), 1, data->profile_file);
    if (res != 1) {
        debug("Error\n");
    }
}

static void save_profile_macros(app_data *data) {
    recorded_macro *macros = data->macro_data->macros;
    int macro_count = data->macro_data->macro_count;

    for (int i = 0; i < macro_count; i++) {
        macro_detail detail = {
            .macro_name_length = strlen(macros[i].name) + 1,
            .event_count = macros[i].generic_event_count,
            .repeat_mode = macros[i].repeat_mode
        };

        fwrite(&detail, sizeof(macro_detail), 1, data->profile_file);
        fwrite(macros[i].name, sizeof(char), detail.macro_name_length, data->profile_file);
        fwrite(macros[i].events, sizeof(generic_macro_event), detail.event_count, data->profile_file);
    }
}

int load_profile_from_file(app_data *data) {
    char *profile_path = application_data_get_file_path(data, data->settings_filename);

    data->profile_file = fopen(profile_path, "rb");
    int res = handle_file_error(data->profile_file, profile_path, false);
    if (res < 0) goto free_path;
    
    load_profile_settings(data, profile_path);
    load_profile_macros(data);

    fclose(data->profile_file);

    free_path:
        free(profile_path);

    return res;
}

int save_profile_to_file(app_data *data) {
    char *profile_path = application_data_get_file_path(data, data->settings_filename);

    data->profile_file = fopen(profile_path, "wb");
    int res = handle_file_error(data->profile_file, profile_path, true);
    if (res < 0) goto free_path;

    save_profile_settings(data);
    save_profile_macros(data);
    
    fclose(data->profile_file);

    free_path:
        free(profile_path);

    return res;
}
