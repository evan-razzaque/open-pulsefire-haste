#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "device/mouse.h"

#include "types.h"
#include "mouse_config.h"

static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    g_simple_action_set_state(action, value);
    printf("%d\n", g_variant_get_int32(value));
    set_polling_rate(data->mouse->dev, g_variant_get_int32(value));
}

static void add_dpi_profile(GSimpleAction* action, GVariant *value, app_data *data) {
    GtkListBoxRow *row = (GtkListBoxRow*) gtk_list_box_row_new();
    gtk_list_box_row_set_activatable(row, false);
    gtk_list_box_row_set_selectable(row, false);

    GtkBox *box = (GtkBox*) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(GTK_WIDGET(box), "dpi-profile");
    gtk_list_box_row_set_child(row, GTK_WIDGET(box));

    GtkCheckButton *radio_button_selection = (GtkCheckButton*) gtk_check_button_new();
    gtk_check_button_set_group(radio_button_selection, data->sensor_data.check_button_default_dpi_profile);

    GtkScale *scale_dpi_value = (GtkScale*) gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 2, 16, 1);
    gtk_widget_set_hexpand(GTK_WIDGET(scale_dpi_value), true);
    gtk_scale_set_value_pos(scale_dpi_value, GTK_POS_LEFT);
    gtk_scale_set_draw_value(scale_dpi_value, true);
    gtk_scale_set_digits(scale_dpi_value, 0);

    GtkEditableLabel *editable_label_dpi_value = (GtkEditableLabel*) gtk_editable_label_new("1000");
    gtk_editable_set_max_width_chars(GTK_EDITABLE(editable_label_dpi_value), 6);
    
    const GdkRGBA *default_dpi_indicator_color = gtk_color_dialog_button_get_rgba(data->sensor_data.color_dialog_button_default_indicator);

    GtkColorDialogButton *dpi_indicator_color = (GtkColorDialogButton*) gtk_color_dialog_button_new(gtk_color_dialog_new());
    gtk_color_dialog_button_set_rgba(dpi_indicator_color, default_dpi_indicator_color);

    gtk_box_append(box, GTK_WIDGET(radio_button_selection));
    gtk_box_append(box, GTK_WIDGET(scale_dpi_value));
    gtk_box_append(box, GTK_WIDGET(editable_label_dpi_value));
    gtk_box_append(box, GTK_WIDGET(dpi_indicator_color));

    GtkListBox *list_box = data->sensor_data.list_box_dpi_profiles;
    gtk_list_box_append(list_box, GTK_WIDGET(row));
    data->sensor_data.dpi_profile_count++;

    if (data->sensor_data.dpi_profile_count == 5) {
        gtk_widget_set_visible(data->sensor_data.button_add_dpi_profile, false);
    }
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    data->sensor_data.button_add_dpi_profile = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddDpiProfile"));
    data->sensor_data.list_box_dpi_profiles = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "listBoxDpiProfiles")));
    data->sensor_data.check_button_default_dpi_profile = GTK_CHECK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "defaultDpiProfile")));
    data->sensor_data.color_dialog_button_default_indicator = (GtkColorDialogButton*) gtk_builder_get_object(builder, "defaultIndicatorColor");

    const GActionEntry entries[] = {
        {.name = "change-polling-rate", .change_state = (g_action) change_polling_rate, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, .state = "3"},
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_profile}
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}