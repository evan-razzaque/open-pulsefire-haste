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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "config_sensor.h"

#include "device/mouse.h"
#include "device/sensor.h"

#include "types.h"
#include "defs.h"

#include "templates/dpi_profile_config.h"

#include "mouse_profile_storage.h"

/**
 * @brief Updates the mouse's dpi settings.
 * 
 * @param data Application wide data structure
 */
static void update_dpi_settings(app_data *data) {
    g_mutex_lock(data->mouse->mutex);
    save_dpi_settings(data->mouse->dev, &data->profile->dpi_config, data->profile->lift_off_distance);
    g_mutex_unlock(data->mouse->mutex);
}

/**
 * @brief Updates a dpi profile.
 * 
 * @param self The DpiProfileConfig instance representing the dpi profile
 * @param profile_index The index of the dpi_profile
 * @param dpi_value The dpi value
 * @param indicator The profile indicator color
 * @param data Application wide data structure
 */
static void update_dpi_profile_data(DpiProfileConfig *self, byte profile_index, int dpi_value, GdkRGBA *indicator, app_data *data) {
    dpi_profile *profile = data->profile->dpi_config.profiles + profile_index;

    if (profile == NULL) return;
    
    profile->dpi_value = (uint16_t) dpi_value;
    profile->indicator.red = indicator->red * 255;
    profile->indicator.green = indicator->green * 255;
    profile->indicator.blue = indicator->blue * 255;
    
    update_dpi_settings(data);
}

/**
 * @brief Changes the polling rate of the mouse.
 * 
 * @param action The simple action containing the check button state for the polling rate
 * @param value The polling rate value
 * @param data Application wide data structure
 */
static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    byte polling_rate_value = g_variant_get_byte(value);
    g_simple_action_set_state(action, value);

    if (data->profile->polling_rate_value == polling_rate_value) return;
    
    g_mutex_lock(data->mouse->mutex);

    data->profile->polling_rate_value = polling_rate_value;
    set_polling_rate(data->mouse->dev, polling_rate_value);

    g_mutex_unlock(data->mouse->mutex);
}

/**
 * @brief Change the lift off distance of the mouse.
 * 
 * @param action The simple action containing the check button state for the lift off distance
 * @param value The lift off distance value (in milimeters)
 * @param data Application wide data structure
 */
static void change_lift_off_distance(GSimpleAction* action, GVariant *value, app_data *data) {
    byte lift_off_distance = g_variant_get_byte(value);
    g_simple_action_set_state(action, value);

    if (data->profile->lift_off_distance == lift_off_distance) return;

    data->profile->lift_off_distance = lift_off_distance;
    update_dpi_settings(data);
}

/**
 * @brief Selects a dpi profile.
 * 
 * @param action The simple action containing the state for the selected dpi profile index
 * @param value_profile_index The index of the dpi profile to select
 * @param data Application wide data structure
 */
static void select_dpi_profile(GSimpleAction *action, GVariant *value_profile_index, app_data *data) {
    byte profile_index = g_variant_get_byte(value_profile_index);
    g_simple_action_set_state(action, value_profile_index);

    dpi_settings *dpi_config = &data->profile->dpi_config;
    
    if (dpi_config->selected_profile == profile_index) return;

    /* Prevents `update_dpi_profile_selection` from being called 
    when a dpi profile change is read directly from the mouse
    when `update_dpi_settings` is called */
    data->sensor_data->user_changed_dpi_profile = true; 

    data->profile->dpi_config.selected_profile = profile_index;
    update_dpi_settings(data);
}

/**
 * @brief Create a dpi profile row widget from a dpi profile 
 * 
 * @param profile_index The index of the profile
 * @param profile The dpi profile
 * @param data Application wide data structure
 * 
 * @return a DpiProfileConfig widget
 */
static DpiProfileConfig* create_dpi_profile_row(byte profile_index, dpi_profile *profile, app_data *data) {
    dpi_settings *dpi_config = &data->profile->dpi_config;

    DpiProfileConfig *self = dpi_profile_config_new(
        data->sensor_data->check_button_group_dpi_profile,
        profile_index
    ); 

    if (profile != NULL) {
        GdkRGBA rgb = {
            .red = profile->indicator.red / 255.0,
            .green = profile->indicator.green / 255.0,
            .blue = profile->indicator.blue / 255.0,
        };

        dpi_profile_config_set_dpi_value(self, profile->dpi_value);
        dpi_profile_config_set_indicator(self, &rgb);
    }

    g_signal_connect(self, "profile-updated", G_CALLBACK(update_dpi_profile_data), data);
    
    GtkListBox *list_box_dpi_profiles = data->sensor_data->list_box_dpi_profiles;
    gtk_list_box_append(list_box_dpi_profiles, GTK_WIDGET(self));

    gtk_widget_set_visible(
        data->sensor_data->button_add_dpi_profile,
        profile_index < (G_N_ELEMENTS(dpi_config->profiles) - 1)
    );

    dpi_profile_config_delete_button_set_enabled(
        DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(list_box_dpi_profiles, 0)),
        profile_index > 0
    );
    
    return self;
}

/**
 * @brief Creates a dpi profile.
 * 
 * @param action Unused
 * @param value Unused
 * @param data Application wide data structure
 */
static void add_dpi_profile(GSimpleAction* action, GVariant *value, app_data *data) {
    dpi_settings *dpi_config = &data->profile->dpi_config;

    DpiProfileConfig *dpi_profile_row = create_dpi_profile_row(dpi_config->profile_count, NULL, data);
    
    uint16_t dpi_value = dpi_profile_config_get_dpi_value(dpi_profile_row);
    const GdkRGBA *rgba = dpi_profile_config_get_indicator(dpi_profile_row);
    
    dpi_config->profiles[dpi_config->profile_count] = (dpi_profile) {
        .dpi_value = dpi_value, 
        .indicator = {
            .red = rgba->red * 255,
            .green = rgba->green * 255,
            .blue = rgba->blue * 255
        }
    };

    dpi_config->enabled_profile_bit_mask += 1 << dpi_config->profile_count; // Sets the bit to allow an additional profile to be enabled
    dpi_config->profile_count++;

    update_dpi_settings(data);
}

void update_dpi_profile_selection(dpi_profile_selection_args *args) {
    dpi_settings *dpi_config = &args->data->profile->dpi_config;
    GtkListBox *list_box_dpi_profiles = args->data->sensor_data->list_box_dpi_profiles;

    GtkListBoxRow *dpi_profile_row = gtk_list_box_get_row_at_index(
        list_box_dpi_profiles,
        args->index
    );
    
    dpi_config->selected_profile = args->index;
    dpi_profile_config_activate(DPI_PROFILE_CONFIG(dpi_profile_row));

    if (args->free_func != NULL) {
        args->free_func(args);
    }
}

/**
 * @brief A function to delete a dpi profile
 * 
 * @param action Unused
 * @param value_profile_index The index of the dpi profile to delete
 * @param data Application wide data structure
 */
static void delete_dpi_profile(GSimpleAction* action, GVariant *value_profile_index, app_data *data) {
    byte profile_index = g_variant_get_byte(value_profile_index);
    dpi_settings *dpi_config = &data->profile->dpi_config;
    config_sensor_data *sensor_data = data->sensor_data;

    array_delete_element(dpi_config->profiles, dpi_config->profile_count, profile_index);

    dpi_config->enabled_profile_bit_mask >>= 1;
    update_dpi_settings(data);

    if (
        dpi_config->selected_profile == dpi_config->profile_count
        || dpi_config->selected_profile > profile_index
    ) {
        dpi_config->selected_profile--;
    }

    GtkListBox *list_box = sensor_data->list_box_dpi_profiles;
    
    DpiProfileConfig *dpi_profile_row = DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(list_box, profile_index));
    dpi_profile_config_remove_check_button_group(dpi_profile_row);
    
    gtk_list_box_remove(list_box, GTK_WIDGET(gtk_list_box_get_row_at_index(list_box, profile_index)));

    for (byte i = 0; i < dpi_config->profile_count; i++) {
        dpi_profile_row = DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(list_box, i));
        dpi_profile_config_set_index(dpi_profile_row, i);

        if (i == dpi_config->selected_profile) {
            dpi_profile_config_activate(dpi_profile_row);
        }
    }

    dpi_profile_config_delete_button_set_enabled(
        dpi_profile_row, 
        dpi_config->profile_count > 1
    );

    gtk_widget_set_visible(sensor_data->button_add_dpi_profile, true);
}

void create_dpi_profile_rows(dpi_settings *dpi_config, app_data *data) {
    gtk_list_box_remove_all(data->sensor_data->list_box_dpi_profiles);

    for (byte i = 0; i < dpi_config->profile_count; i++) {
        create_dpi_profile_row(i, dpi_config->profiles + i, data);
    }
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    data->sensor_data->button_add_dpi_profile = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddDpiProfile"));
    data->sensor_data->list_box_dpi_profiles = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "listBoxDpiProfiles")));
    data->sensor_data->check_button_group_dpi_profile = GTK_CHECK_BUTTON(gtk_check_button_new());

    create_dpi_profile_rows(&data->profile->dpi_config, data);

    char selected_profile_state[10];
    sprintf(selected_profile_state, "byte %d", data->profile->dpi_config.selected_profile);

    char polling_rate_string[10];
    sprintf(polling_rate_string, "byte %d", data->profile->polling_rate_value);

    char lift_off_distance_string[10];
    sprintf(lift_off_distance_string, "byte %d", data->profile->lift_off_distance);
    
    const GActionEntry entries[] = {
        {.name = CHANGE_POLLING_RATE, .change_state = (g_action) change_polling_rate, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE, .state = polling_rate_string},
        {.name = CHANGE_LIFT_OFF_DISTANCE, .change_state = (g_action) change_lift_off_distance, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE, lift_off_distance_string},
        {.name = SELECT_DPI_PROFILE, .change_state = (g_action) select_dpi_profile, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE, .state = selected_profile_state},
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_profile},
        {.name = "delete-dpi-profile", .activate = (g_action) delete_dpi_profile, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE}
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}
