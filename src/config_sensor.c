#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "device/mouse.h"

#include "types.h"
#include "mouse_config.h"

G_DECLARE_FINAL_TYPE(DpiProfileConfig, dpi_profile_config, DPI, PROFILE_CONFIG, GtkListBoxRow)

#define DPI_TYPE_PROFILE_CONFIG (dpi_profile_config_get_type())
#define DPI_PROFILE_CONFIG(inst) (G_TYPE_CHECK_INSTANCE_CAST ((inst), DPI_TYPE_PROFILE_CONFIG, DpiProfileConfig))

struct _DpiProfileConfig {
    GtkListBoxRow parent_type;

    GtkCheckButton *check_button;
    GtkRange *range_dpi_value;
    GtkEditable *editable_dpi_value;
    GtkColorDialogButton *color_button_dpi_indicator;
};

G_DEFINE_TYPE(DpiProfileConfig, dpi_profile_config, GTK_TYPE_LIST_BOX_ROW)

static void dpi_profile_config_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), DPI_TYPE_PROFILE_CONFIG);
    G_OBJECT_CLASS(dpi_profile_config_parent_class)->dispose(gobject);
}

static void update_editable_dpi_value(GtkEditable *self, GtkRange *range_dpi_value) {
    char value[8];
    sprintf(value, "%d", (int) gtk_range_get_value(range_dpi_value) * 100);
    printf("Range: %s\n", value);

    gtk_editable_set_text(self, value);
}

static void update_range_dpi_value(GtkRange *self, gchar* text, gint length, gint* position, GtkEditable *editable_dpi_value) {
    int value = atoi(gtk_editable_get_text(editable_dpi_value));
    printf("Editable: %d\n", value);

    // gtk_range_set_value(self, (double) value);
}

static void dpi_profile_config_class_init(DpiProfileConfigClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = dpi_profile_config_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/com/haste/dpi-profile-config.ui");

    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, check_button);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, range_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, editable_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, color_button_dpi_indicator);

    gtk_widget_class_bind_template_callback(widget_class, update_editable_dpi_value);
    gtk_widget_class_bind_template_callback(widget_class, update_range_dpi_value);
}

static void dpi_profile_config_init(DpiProfileConfig *self) {
    g_type_ensure(DPI_TYPE_PROFILE_CONFIG);
    gtk_widget_init_template(GTK_WIDGET(self));
}

DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group) {
    DpiProfileConfig* self = g_object_new(DPI_TYPE_PROFILE_CONFIG, NULL);
    gtk_check_button_set_group(self->check_button, check_button_group);

    return self;
}

static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    g_simple_action_set_state(action, value);
    set_polling_rate(data->mouse->dev, g_variant_get_int32(value));
}

static void add_dpi_profile(GSimpleAction* action, GVariant *value, app_data *data) {
    DpiProfileConfig *dpi_config = dpi_profile_config_new(data->sensor_data.check_button_group_dpi_profile);
    GtkListBoxRow *row = &dpi_config->parent_type;
    gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, GTK_WIDGET(row));

    gtk_check_button_set_group(dpi_config->check_button, data->sensor_data.check_button_group_dpi_profile);

    data->sensor_data.dpi_profile_count++;

    if (data->sensor_data.dpi_profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    GtkCheckButton *group = GTK_CHECK_BUTTON(gtk_check_button_new());
    data->sensor_data.check_button_group_dpi_profile = group;

    DpiProfileConfig *defaultDpiProfileRow = dpi_profile_config_new(data->sensor_data.check_button_group_dpi_profile);
    gtk_check_button_set_active(defaultDpiProfileRow->check_button, true);
    
    data->sensor_data.button_add_dpi_profile = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddDpiProfile"));
    data->sensor_data.list_box_dpi_profiles = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "listBoxDpiProfiles")));
    data->sensor_data.default_dpi_profile_row = GTK_WIDGET(defaultDpiProfileRow);

    gtk_list_box_append(data->sensor_data.list_box_dpi_profiles, data->sensor_data.default_dpi_profile_row);

    const GActionEntry entries[] = {
        {.name = "change-polling-rate", .change_state = (g_action) change_polling_rate, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, .state = "3"},
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_profile}
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}