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


G_DECLARE_FINAL_TYPE(DpiProfileConfig, dpi_profile_config, DPI, PROFILE_CONFIG, GtkListBoxRow)

#define DPI_TYPE_PROFILE_CONFIG (dpi_profile_config_get_type())
#define DPI_PROFILE_CONFIG(inst) (G_TYPE_CHECK_INSTANCE_CAST ((inst), DPI_TYPE_PROFILE_CONFIG, DpiProfileConfig))

struct _DpiProfileConfig {
    GtkListBoxRow parent_type;

    GtkCheckButton *check_button;
    GtkRange *range_dpi_value;
    GtkSpinButton *spinner_dpi_value;
    GtkColorDialogButton *color_button_dpi_indicator;
};

G_DEFINE_TYPE(DpiProfileConfig, dpi_profile_config, GTK_TYPE_LIST_BOX_ROW)

static void dpi_profile_config_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), DPI_TYPE_PROFILE_CONFIG);
    G_OBJECT_CLASS(dpi_profile_config_parent_class)->dispose(gobject);
}

static void update_spinner_dpi_value(GtkSpinButton *self, GtkRange *range_dpi_value);
static void update_range_dpi_value(GtkRange* self, GtkSpinButton *spinner_dpi_value);

static void update_range_dpi_value(GtkRange* self, GtkSpinButton *spinner_dpi_value) {
    double value = gtk_spin_button_get_value(spinner_dpi_value) / 100;
    
    g_signal_handlers_block_by_func(self, G_CALLBACK(update_spinner_dpi_value), spinner_dpi_value);
    gtk_range_set_value(self, value);
    g_signal_handlers_unblock_by_func(self, G_CALLBACK(update_spinner_dpi_value), spinner_dpi_value);
}

static void update_spinner_dpi_value(GtkSpinButton *self, GtkRange *range_dpi_value) {
    double value = gtk_range_get_value(range_dpi_value) * 100;

    g_signal_handlers_block_by_func(self, G_CALLBACK(update_range_dpi_value), range_dpi_value);
    gtk_spin_button_set_value(self, value);
    g_signal_handlers_unblock_by_func(self, G_CALLBACK(update_range_dpi_value), range_dpi_value);
}

static void dpi_profile_config_class_init(DpiProfileConfigClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = dpi_profile_config_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/com/haste/dpi-profile-config.ui");

    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, check_button);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, range_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, spinner_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, color_button_dpi_indicator);

    gtk_widget_class_bind_template_callback(widget_class, update_spinner_dpi_value);
    gtk_widget_class_bind_template_callback(widget_class, update_range_dpi_value);
}

static void dpi_profile_config_init(DpiProfileConfig *self) {
    g_type_ensure(DPI_TYPE_PROFILE_CONFIG);
    gtk_widget_init_template(GTK_WIDGET(self));
}

DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group, uint16_t dpi_value, color_options indicator, int action_target) {
    DpiProfileConfig* self = g_object_new(DPI_TYPE_PROFILE_CONFIG, NULL);

    printf("%d\n", action_target);

    GtkSpinButton *spin_button = self->spinner_dpi_value;
    GtkWidget *button_decrease_value = gtk_widget_get_next_sibling(
        gtk_widget_get_first_child(GTK_WIDGET(spin_button))
    );
    GtkWidget *button_increase_value = gtk_widget_get_next_sibling(button_decrease_value);

    gtk_widget_set_visible(button_decrease_value, false);
    gtk_widget_set_visible(button_increase_value, false);

    const GdkRGBA rgba = {indicator.red, indicator.green, indicator.blue, 1};

    gtk_spin_button_set_value(self->spinner_dpi_value, (double) dpi_value);
    gtk_color_dialog_button_set_rgba(self->color_button_dpi_indicator, &rgba);

    gtk_check_button_set_group(self->check_button, check_button_group);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->check_button), "i", action_target);

    return self;
}

static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    uint32_t polling_rate_value = g_variant_get_int32(value);

    g_simple_action_set_state(action, value);

    set_polling_rate(data->mouse->dev, g_variant_get_int32(value));
    data->sensor_data.polling_rate_value = polling_rate_value;
}

static void update_dpi_settings(app_data* data, uint32_t lift_off_distance) {
    g_mutex_lock(data->mouse->mutex);
    printf("Dpi Profile: %d\n", data->sensor_data.dpi_config.selected_profile);
    save_dpi_settings(data->mouse->dev, &data->sensor_data.dpi_config, lift_off_distance);
    g_mutex_unlock(data->mouse->mutex);
}

static void change_lift_off_distance(GSimpleAction* action, GVariant *value, app_data *data) {
    uint32_t lift_off_distance = g_variant_get_int32(value);

    g_simple_action_set_state(action, value);

    update_dpi_settings(data, lift_off_distance);
    data->sensor_data.lift_off_distance = lift_off_distance;
}

static void add_dpi_profile(GSimpleAction* action, GVariant *value, app_data *data) {
    dpi_settings *settings = &data->sensor_data.dpi_config;

    DpiProfileConfig *dpi_config_row = dpi_profile_config_new(
        data->sensor_data.check_button_group_dpi_profile, 
        (settings->profile_count + 1) * 800,
        (color_options) {.red = 0xff},
        settings->profile_count
    );

    GtkListBoxRow *row = &dpi_config_row->parent_type;
    gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(row));

    settings->profiles[settings->profile_count] = (dpi_profile) {.dpi_value = (settings->profile_count + 1) * 800, .indicator = {.red = 0xff}};
    settings->profile_count++;

    if (settings->profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }
}

static void select_dpi_profile(GSimpleAction *action, GVariant *profile_number, app_data *data) {
    int selected_profile = g_variant_get_int32(profile_number);
    g_simple_action_set_state(action, profile_number);

    data->sensor_data.dpi_config.selected_profile = selected_profile;
    update_dpi_settings(data, data->sensor_data.lift_off_distance);
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    GtkCheckButton *group = GTK_CHECK_BUTTON(gtk_check_button_new());
    data->sensor_data.check_button_group_dpi_profile = group;

    data->sensor_data.button_add_dpi_profile = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddDpiProfile"));
    data->sensor_data.list_box_dpi_profiles = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "listBoxDpiProfiles")));

    for (int i = 0; i < data->sensor_data.dpi_config.profile_count; i++) {
        DpiProfileConfig *dpi_profile_row = dpi_profile_config_new(
            group,
            data->sensor_data.dpi_config.profiles[i].dpi_value,
            data->sensor_data.dpi_config.profiles[i].indicator,
            i
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
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_profile},
        {.name = "select-dpi-profile", .change_state = (g_action) select_dpi_profile, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, .state = profile_state},
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}