#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "device/mouse.h"
#include "device/sensor.h"

#include "types.h"
#include "mouse_config.h"

#include "./templates/dpi_profile_config.h"

static void update_dpi_settings(app_data* data) {
    g_mutex_lock(data->mouse->mutex);
    save_dpi_settings(data->mouse->dev, &data->sensor_data.dpi_config, data->sensor_data.lift_off_distance);
    g_mutex_unlock(data->mouse->mutex);
}

static void update_dpi_profile_data(DpiProfileConfig *self, byte profile_index, int dpi_value, GdkRGBA *indicator, app_data *data) {
    dpi_profile *profile = data->sensor_data.dpi_config.profiles + profile_index;
    
    profile->dpi_value = (uint16_t) dpi_value;
    profile->indicator.red = indicator->red * 255;
    profile->indicator.green = indicator->green * 255;
    profile->indicator.blue = indicator->blue * 255;
    
    update_dpi_settings(data);
}

static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    byte polling_rate_value = g_variant_get_byte(value);
    g_simple_action_set_state(action, value);

    set_polling_rate(data->mouse->dev, g_variant_get_byte(value));
    data->sensor_data.polling_rate_value = polling_rate_value;
}

static void change_lift_off_distance(GSimpleAction* action, GVariant *value, app_data *data) {
    byte lift_off_distance = g_variant_get_byte(value);
    g_simple_action_set_state(action, value);

    data->sensor_data.lift_off_distance = lift_off_distance;
    update_dpi_settings(data);
}

static void add_dpi_profile(GSimpleAction* action, GVariant *value, app_data *data) {
    dpi_settings *dpi_config = &data->sensor_data.dpi_config;
    
    DpiProfileConfig *dpi_profile_row = dpi_profile_config_new(
        data->sensor_data.check_button_group_dpi_profile,
        dpi_config->profile_count
    );

    dpi_profile_config_set_active(dpi_profile_row, false);
    
    g_signal_connect(dpi_profile_row, "profile-updated", G_CALLBACK(update_dpi_profile_data), data);

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
    
    dpi_profile_config_delete_button_set_enabled(
        DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(
            data->sensor_data.list_box_dpi_profiles, 0)
        ),
        true
    );
    
    if (dpi_config->profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }

    gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(dpi_profile_row));

    update_dpi_settings(data);
}

static void select_dpi_profile(GSimpleAction *action, GVariant *value_profile_index, app_data *data) {
    byte profile_index = g_variant_get_byte(value_profile_index);
    g_simple_action_set_state(action, value_profile_index);

    data->sensor_data.dpi_config.selected_profile = profile_index;
    update_dpi_settings(data);
}

static void delete_dpi_profile(GSimpleAction* action, GVariant *value_profile_index, app_data *data) {
    byte profile_index = g_variant_get_byte(value_profile_index);
    dpi_settings *dpi_config = &data->sensor_data.dpi_config;
    config_sensor_data *sensor_data = &data->sensor_data;

    array_delete_element(dpi_config->profiles, dpi_config->profile_count, profile_index);

    dpi_config->enabled_profile_bit_mask >>= 1;
    update_dpi_settings(data);

    if (dpi_config->selected_profile == dpi_config->profile_count
        || dpi_config->selected_profile > profile_index
    ) {
        dpi_config->selected_profile--;
    }

    GtkListBox *list_box = sensor_data->list_box_dpi_profiles;
    gtk_list_box_remove(list_box, GTK_WIDGET(gtk_list_box_get_row_at_index(list_box, profile_index)));
    
    DpiProfileConfig *dpi_profile_row = NULL;

    for (byte i = 0; i < dpi_config->profile_count; i++) {
        dpi_profile_row = DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(list_box, i));
        dpi_profile_config_set_index(dpi_profile_row, i);

        if (i == dpi_config->selected_profile) {
            dpi_profile_config_set_active(dpi_profile_row, true);
        }
    }

    if (dpi_config->profile_count == 1) {
        dpi_profile_config_delete_button_set_enabled(dpi_profile_row, false);
    }

    gtk_widget_set_visible(sensor_data->button_add_dpi_profile, true);
}

static void load_dpi_profile_rows(app_data* data, GtkCheckButton* check_button_group) {
    dpi_settings* dpi_config = &data->sensor_data.dpi_config;

    for (int i = 0; i < data->sensor_data.dpi_config.profile_count; i++) {
        DpiProfileConfig* dpi_profile_row = dpi_profile_config_new(check_button_group, i);
        dpi_profile_config_set_dpi_value(dpi_profile_row, dpi_config->profiles[i].dpi_value);

        color_options* indicator = &dpi_config->profiles[i].indicator;

        GdkRGBA rgb = {
            .red = indicator->red / 255.0,
            .green = indicator->green / 255.0,
            .blue = indicator->blue / 255.0
        };

        dpi_profile_config_set_indicator(dpi_profile_row, &rgb);

        g_signal_connect(dpi_profile_row, "profile-updated", G_CALLBACK(update_dpi_profile_data), data);
        gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(dpi_profile_row));
    }
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    GtkCheckButton *check_button_group = GTK_CHECK_BUTTON(gtk_check_button_new());
    data->sensor_data.check_button_group_dpi_profile = check_button_group;

    data->sensor_data.button_add_dpi_profile = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddDpiProfile"));
    data->sensor_data.list_box_dpi_profiles = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "listBoxDpiProfiles")));
    
    load_dpi_profile_rows(data, check_button_group);

    byte profile_count = data->sensor_data.dpi_config.profile_count;

    if (profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    } else if (profile_count == 1) {
        GtkListBox *list_box = data->sensor_data.list_box_dpi_profiles;
        DpiProfileConfig *dpi_profile_row = DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(list_box, 0));
        
        dpi_profile_config_delete_button_set_enabled(dpi_profile_row, false);
    }

    char profile_state[10];
    sprintf(profile_state, "byte %d", data->sensor_data.dpi_config.selected_profile);

    char polling_rate_string[10];
    sprintf(polling_rate_string, "byte %d", data->sensor_data.polling_rate_value);

    char lift_off_distance_string[10];
    sprintf(lift_off_distance_string, "byte %d", data->sensor_data.lift_off_distance);
    
    const GActionEntry entries[] = {
        {.name = "change-polling-rate", .change_state = (g_action) change_polling_rate, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE, .state = polling_rate_string},
        {.name = "change-lift-off-distance", .change_state = (g_action) change_lift_off_distance, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE, lift_off_distance_string},
        {.name = "select-dpi-profile", .change_state = (g_action) select_dpi_profile, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE, .state = profile_state},
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_profile},
        {.name = "delete-dpi-profile", .activate = (g_action) delete_dpi_profile, .parameter_type = (const char*) G_VARIANT_TYPE_BYTE}
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}
