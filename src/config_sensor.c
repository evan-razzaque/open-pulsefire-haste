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

DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group, dpi_profile *profile, int action_target, app_data *data) {
    DpiProfileConfig* self = g_object_new(DPI_TYPE_PROFILE_CONFIG, NULL);
    self->profile_index = action_target;

    data->sensor_data.check_buttons_dpi_profile[action_target] = self->check_button;
    
    self->gesture_click_controller = GTK_GESTURE_CLICK(gtk_gesture_click_new());
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(self->gesture_click_controller), GTK_PHASE_CAPTURE);
    gtk_widget_add_controller(
        gtk_widget_get_parent(GTK_WIDGET(self->range_dpi_value)),
        GTK_EVENT_CONTROLLER(self->gesture_click_controller)
    );

    uint16_t dpi_value = profile->dpi_value;
    color_options indicator = profile->indicator;

    GtkSpinButton *spin_button = self->spinner_dpi_value;
    GtkWidget *button_decrease_value = gtk_widget_get_next_sibling(
        gtk_widget_get_first_child(GTK_WIDGET(spin_button))
    );
    GtkWidget *button_increase_value = gtk_widget_get_next_sibling(button_decrease_value);

    gtk_widget_set_visible(button_decrease_value, false);
    gtk_widget_set_visible(button_increase_value, false);

    const GdkRGBA rgba = {
        .red = indicator.red / 255.0,
        .green = indicator.green / 255.0,
        .blue = indicator.blue / 255.0, 
        .alpha = 1
    };
    
    gtk_spin_button_set_value(self->spinner_dpi_value, (double) dpi_value);
    gtk_color_dialog_button_set_rgba(self->color_button_dpi_indicator, &rgba);

    gtk_check_button_set_group(self->check_button, check_button_group);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->check_button), "i", action_target);

    g_signal_connect(self->color_button_dpi_indicator, "notify::rgba", G_CALLBACK(update_dpi_profile_indicator), data);
    g_signal_connect(self->spinner_dpi_value, "value-changed", G_CALLBACK(update_dpi_value_spinner), data);
    g_signal_connect(self->gesture_click_controller, "released", G_CALLBACK(update_dpi_value_range), data);
    g_signal_connect(self->gesture_click_controller, "pressed", G_CALLBACK(disable_update_dpi_value_spinner), data);

    return self;
}

static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    uint32_t polling_rate_value = g_variant_get_int32(value);
    g_simple_action_set_state(action, value);

    set_polling_rate(data->mouse->dev, g_variant_get_int32(value));
    data->sensor_data.polling_rate_value = polling_rate_value;
}

static void change_lift_off_distance(GSimpleAction* action, GVariant *value, app_data *data) {
    uint32_t lift_off_distance = g_variant_get_int32(value);
    g_simple_action_set_state(action, value);

    data->sensor_data.lift_off_distance = lift_off_distance;
    update_dpi_settings(data);
}

static void add_dpi_profile_row(GSimpleAction* action, GVariant *value, app_data *data) {
    dpi_settings *settings = &data->sensor_data.dpi_config;
    settings->profiles[settings->profile_count] = (dpi_profile) {.dpi_value = (settings->profile_count + 1) * 800, .indicator = {.red = 0xff}};

    DpiProfileConfig *dpi_config_row = dpi_profile_config_new(
        data->sensor_data.check_button_group_dpi_profile, 
        &settings->profiles[settings->profile_count],
        settings->profile_count,
        data
    );

    GtkListBoxRow *row = &dpi_config_row->parent_type;
    gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(row));

    settings->enabled_profile_bit_mask += 1 << settings->profile_count; // Sets the bit to allow an additional profile to be enabled
    settings->profile_count++;

    if (settings->profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }

    update_dpi_settings(data);
}

static void select_dpi_profile(GSimpleAction *action, GVariant *profile_number, app_data *data) {
    int selected_profile = g_variant_get_int32(profile_number);

    g_mutex_lock(data->mouse->mutex);
    g_simple_action_set_state(action, profile_number);
    g_mutex_unlock(data->mouse->mutex);

    data->sensor_data.dpi_config.selected_profile = selected_profile;
    update_dpi_settings(data);
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

        if (i == data->sensor_data.dpi_config.selected_profile) {
            gtk_check_button_set_active(dpi_profile_row->check_button, true);
        }

        gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(dpi_profile_row));
    }

    if (data->sensor_data.dpi_config.profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }

    char profile_state[8];
    sprintf(profile_state, "%d", data->sensor_data.dpi_config.selected_profile);

    const GActionEntry entries[] = {
        {.name = "change-polling-rate", .change_state = (g_action) change_polling_rate, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, .state = "3"},
        {.name = "change-lift-off-distance", .change_state = (g_action) change_lift_off_distance, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, "1"},
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_profile_row},
        {.name = "select-dpi-profile", .change_state = (g_action) select_dpi_profile, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, .state = profile_state},
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}