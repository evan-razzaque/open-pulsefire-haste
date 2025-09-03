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
#include "defs.h"

/**
 * @brief A struct used for reading and writing information about a `recorded_macro` with the disk.
 */
struct macro_detail {
    size_t macro_name_length;
    int event_count;
    REPEAT_MODE repeat_mode;
} typedef macro_detail;

/**
 * @brief A function to handle errors with open_profile_file() and remove().
 * 
 * @param file The accessed file that caused the error. Should be NULL when using remove()
 * @param filename The path of the file
 * @param file_should_exist Whether or not this file should exist or not
 * @return int 0 if there were no errors or -1 if there was an error
 */
static int handle_file_error(FILE *file, const char* path, bool file_should_exist) {
    int error = errno;
    
    if (error == ENOENT && !file_should_exist) return 0;

    if (file == NULL && error != 0) {
        printf("Error accessing %s: %s\n", path, strerror(error));
        return -1;
    }

    return 0;
}

int create_data_directory() {
    char *app_data_path = g_strdup_printf("%s" PATH_SEP APP_DIR, g_get_user_data_dir());
    
    int res = g_mkdir_with_parents(app_data_path, S_IRWXU);
    if (res == 0) {
        chdir(app_data_path);
        res = g_mkdir_with_parents(PROFILE_DIR, S_IRWXU);
    }
    
    free(app_data_path);

    return res;
}

/**
 * @brief Opens a mouse profile file.
 * 
 * @param name The name of the profile
 * @param modes The file mode
 * @return A FILE* if the file was opened or NULL if there was an error
 */
FILE* open_profile_file(const char *name, const char *modes) {
    char *profile_path = g_strdup_printf(PROFILE_DIR "%s" PROFILE_EXTENSION, name);
    
    FILE *file = fopen(profile_path, modes);
    free(profile_path);

    return file;
}

/**
 * @brief Removes a mouse profile file.
 * 
 * @param name The name of the profile
 * @return 0 if the file was removed or -1 if there was an error
 */
int remove_profile_file(const char *name) {
    char *profile_path = g_strdup_printf(PROFILE_DIR "%s" PROFILE_EXTENSION, name);
    
    int res = remove(profile_path);
    free(profile_path);

    return res;
}

/**
 * @brief Creates a `mouse_profile`.
 * 
 * @param settings The mouse_profile object
 * @param profile_path The path to save the profile to
 * @param data Application wide data structure
 * @return A `mouse_profile` object if the profile was created or NULL if an error has occured
 */
static mouse_profile* create_profile(const char *profile_path, app_data *data) {
    mouse_profile *profile = malloc(sizeof(mouse_profile));
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
        .polling_rate_value = POLLING_RATE_1000HZ,
        .lift_off_distance = LIFT_OFF_DISTANCE_LOW, // TODO: Find default value
        .macros = malloc(sizeof(recorded_macro)),
        .macro_count = 0,
        .macro_indices = {-1, -1, -1, -1, -1, -1}
    };
    
    data->macro_data->macro_array_size = 1;
    data->profile_file = open_profile_file(profile_path, "wb");

    int res = handle_file_error(data->profile_file, profile_path, true);
    if (res < 0) goto free_profile;

    res = fwrite(profile, sizeof(mouse_profile), 1, data->profile_file);

    if (res != 1) {
        debug("Error\n");
        fclose(data->profile_file);
        goto free_profile;
    }

    return profile;

    free_profile:
        free(profile);
        return NULL;
}

void destroy_profile(mouse_profile *profile) {
    int macro_count = profile->macro_count;

    for (int i = 0; i < macro_count; i++) {
        recorded_macro *macro = &profile->macros[i];
        free(macro->name);
        free(macro->events);
    }

    free(profile->macros);
    free(profile);
}

int delete_profile(const char *name, app_data *data) {
    const char *profile_path = name;
    remove(profile_path);

    int res = handle_file_error(NULL, profile_path, true);
    if (res == 0) {
        g_hash_table_remove(data->mouse_profiles, name);
    }

    return res;
}

static int load_profile_settings(mouse_profile *profile, app_data *data) {
    int res = fread(profile, sizeof(mouse_profile), 1, data->profile_file);

    if (res != 1) {
        debug("Error: %d\n", res);
        return -1;
    }

    return 0;
}

static void load_profile_macros(mouse_profile *profile, app_data *data) {
    data->macro_data->macro_array_size = MAX(profile->macro_count, 1);
    profile->macros = malloc(sizeof(recorded_macro) * data->macro_data->macro_array_size);

    recorded_macro *macros = profile->macros;

    for (int i = 0; i < profile->macro_count; i++) {
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

mouse_profile* load_profile_from_file(const char *name, app_data *data) {
    mouse_profile *profile = NULL;
    
    data->profile_file = open_profile_file(name, "rb");
    int res = handle_file_error(data->profile_file, name, false);
    if (res < 0) return profile;

    if (data->profile_file == NULL) {
        profile = create_profile(name, data);
        if (profile == NULL) {
            debug("Error: %d\n", res);
            return profile;
        }
    } else {
        profile = malloc(sizeof(mouse_profile));
        
        load_profile_settings(profile, data);
        load_profile_macros(profile, data);
    }
    
    g_hash_table_insert(data->mouse_profiles, g_strdup(name), profile);

    fclose(data->profile_file);

    return profile;
}

static int save_profile_settings(mouse_profile *profile, app_data *data) {
    recorded_macro *macros = profile->macros;

    profile->macros = NULL;
    int res = fwrite(profile, sizeof(mouse_profile), 1, data->profile_file);
    profile->macros = macros;

    if (res != 1) {
        debug("Error %s", "f");
        return -1;
    }

    return 0;
}

static void save_profile_macros(mouse_profile *profile, app_data *data) {
    recorded_macro *macros = profile->macros;
    int macro_count = profile->macro_count;

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

int save_profile_to_file(const char *name, mouse_profile *profile, app_data *data) {
    data->profile_file = open_profile_file(name, "wb");
    int res = handle_file_error(data->profile_file, name, true);
    
    if (res == 0) {
        save_profile_settings(profile, data);
        save_profile_macros(profile, data);

        fclose(data->profile_file);
    }

    return res;
}

int switch_profile(const char *name, app_data *data) {
    mouse_profile *profile = g_hash_table_lookup(data->mouse_profiles, name);

    if (profile == NULL) {
        profile = load_profile_from_file(name, data);
        if (profile == NULL) return -1;
    }

    data->profile = profile;
    return 0;
}
