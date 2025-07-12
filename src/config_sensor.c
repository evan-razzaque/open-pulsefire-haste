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

static void update_dpi_profile_indicator(GtkColorDialogButton *self, GParamSpec *param, app_data *data) {
    const GdkRGBA *rgba = gtk_color_dialog_button_get_rgba(self);
    
    DpiProfileConfig *profile_row = DPI_PROFILE_CONFIG(gtk_widget_get_parent(
        gtk_widget_get_parent(GTK_WIDGET(self))
    ));
    
    data->sensor_data.dpi_config.profiles[profile_row->profile_index].indicator = (color_options) {
        .red = rgba->red * 255,
        .green = rgba->green * 255,
        .blue = rgba->blue * 255
    };

    update_dpi_settings(data);
}

static void update_dpi_value_spinner(GtkWidget *self, app_data *data) {
    DpiProfileConfig *profile_row = DPI_PROFILE_CONFIG(gtk_widget_get_parent(gtk_widget_get_parent(self)));
    u_int16_t dpi_value = (int) gtk_spin_button_get_value(profile_row->spinner_dpi_value);

    data->sensor_data.dpi_config.profiles[profile_row->profile_index].dpi_value = dpi_value;
    update_dpi_settings(data);
}

static void update_dpi_value_range(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, app_data *data) {
    DpiProfileConfig *profile_row = DPI_PROFILE_CONFIG(gtk_widget_get_parent(
        gtk_widget_get_parent(
            gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self))
        )
    ));

    g_signal_handlers_unblock_by_func(profile_row->spinner_dpi_value, update_dpi_value_spinner, data);

    uint16_t dpi_value = (int) gtk_range_get_value(profile_row->range_dpi_value) * 100;

    data->sensor_data.dpi_config.profiles[profile_row->profile_index].dpi_value = dpi_value;
    update_dpi_settings(data);
}

static void disable_update_dpi_value_spinner(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, app_data *data) {
    DpiProfileConfig *profile_row = DPI_PROFILE_CONFIG(gtk_widget_get_parent(
        gtk_widget_get_parent(
            gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self))
        )
    ));

    g_signal_handlers_block_by_func(profile_row->spinner_dpi_value, update_dpi_value_spinner, data);
}

DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group, dpi_profile *profile, int profile_index, app_data *data) {
    DpiProfileConfig* self = g_object_new(DPI_TYPE_PROFILE_CONFIG, NULL);
    self->profile_index = profile_index;

    data->sensor_data.check_buttons_dpi_profile[profile_index] = self->check_button;
    
    self->gesture_click_controller = GTK_GESTURE_CLICK(gtk_gesture_click_new());
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(self->gesture_click_controller), GTK_PHASE_CAPTURE);
    gtk_widget_add_controller(
        GTK_WIDGET(self->box_range_dpi_value),
        GTK_EVENT_CONTROLLER(self->gesture_click_controller)
    );

    uint16_t dpi_value = profile->dpi_value;
    color_options indicator = profile->indicator;

    const GdkRGBA rgba = {
        .red = indicator.red / 255.0,
        .green = indicator.green / 255.0,
        .blue = indicator.blue / 255.0, 
        .alpha = 1
    };
    
    gtk_spin_button_set_value(self->spinner_dpi_value, (double) dpi_value);
    gtk_color_dialog_button_set_rgba(self->color_button_dpi_indicator, &rgba);

    gtk_check_button_set_group(self->check_button, check_button_group);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->check_button), "y", profile_index);

    g_signal_connect(self->color_button_dpi_indicator, "notify::rgba", G_CALLBACK(update_dpi_profile_indicator), data);
    g_signal_connect(self->spinner_dpi_value, "value-changed", G_CALLBACK(update_dpi_value_spinner), data);
    g_signal_connect(self->gesture_click_controller, "released", G_CALLBACK(update_dpi_value_range), data);
    g_signal_connect(self->gesture_click_controller, "pressed", G_CALLBACK(disable_update_dpi_value_spinner), data);

    return self;
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
    dpi_config->profiles[dpi_config->profile_count] = (dpi_profile) {.dpi_value = (dpi_config->profile_count + 1) * 800, .indicator = {.red = 0xff}};

    DpiProfileConfig *dpi_profile_row = dpi_profile_config_new(
        data->sensor_data.check_button_group_dpi_profile, 
        &dpi_config->profiles[dpi_config->profile_count],
        dpi_config->profile_count,
        data
    );

    gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(&dpi_profile_row->parent_instance));

    dpi_config->enabled_profile_bit_mask += 1 << dpi_config->profile_count; // Sets the bit to allow an additional profile to be enabled
    dpi_config->profile_count++;

    if (dpi_config->profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }

    update_dpi_settings(data);
}

static void select_dpi_profile(GSimpleAction *action, GVariant *value_profile_index, app_data *data) {
    
    byte profile_index = g_variant_get_byte(value_profile_index);
    g_simple_action_set_state(action, value_profile_index);
    printf("Profile %d\n", profile_index);

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
        gtk_actionable_set_action_target(GTK_ACTIONABLE(dpi_profile_row), "y", i);
        dpi_profile_row->profile_index = i;

        sensor_data->check_buttons_dpi_profile[i] = dpi_profile_row->check_button;
        gtk_actionable_set_action_target(GTK_ACTIONABLE(dpi_profile_row->check_button), "y", i);
    }

    if (dpi_config->profile_count == 1) {
        gtk_widget_set_sensitive(GTK_WIDGET(dpi_profile_row->button_delete_profile), false);
    }

    gtk_check_button_set_active(sensor_data->check_buttons_dpi_profile[dpi_config->selected_profile], true);
    gtk_widget_set_visible(sensor_data->button_add_dpi_profile, true);
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    GtkCheckButton *group = GTK_CHECK_BUTTON(gtk_check_button_new());
    data->sensor_data.check_button_group_dpi_profile = group;

    data->sensor_data.button_add_dpi_profile = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddDpiProfile"));
    data->sensor_data.list_box_dpi_profiles = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "listBoxDpiProfiles")));

    for (int i = 0; i < data->sensor_data.dpi_config.profile_count; i++) {
        DpiProfileConfig *dpi_profile_row = dpi_profile_config_new(
            group,
            &data->sensor_data.dpi_config.profiles[i],
            i,
            data
        );

        gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(dpi_profile_row));
    }

    byte profile_count = data->sensor_data.dpi_config.profile_count;

    if (profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    } else if (profile_count == 1) {
        GtkListBox *list_box = data->sensor_data.list_box_dpi_profiles;
        DpiProfileConfig *dpi_profile_row = DPI_PROFILE_CONFIG(gtk_list_box_get_row_at_index(list_box, 0));

        gtk_widget_set_sensitive(GTK_WIDGET(dpi_profile_row->button_delete_profile), false);
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