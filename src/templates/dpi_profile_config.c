#include <gtk/gtk.h>

#include "dpi_profile_config.h"

G_DEFINE_TYPE(DpiProfileConfig, dpi_profile_config, GTK_TYPE_LIST_BOX_ROW)

static void dpi_profile_config_update_spinner_dpi_value(GtkSpinButton *self, GtkRange *range_dpi_value);
static void dpi_profile_config_update_range_dpi_value(GtkRange* self, GtkSpinButton *spinner_dpi_value);

static void dpi_profile_config_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), DPI_TYPE_PROFILE_CONFIG);
    G_OBJECT_CLASS(dpi_profile_config_parent_class)->dispose(gobject);
}

static void dpi_profile_config_update_range_dpi_value(GtkRange* self, GtkSpinButton *spinner_dpi_value) {
    double value = gtk_spin_button_get_value(spinner_dpi_value) / 100;
    
    g_signal_handlers_block_by_func(self, G_CALLBACK(dpi_profile_config_update_spinner_dpi_value), spinner_dpi_value);
    gtk_range_set_value(self, value);
    g_signal_handlers_unblock_by_func(self, G_CALLBACK(dpi_profile_config_update_spinner_dpi_value), spinner_dpi_value);
}

static void dpi_profile_config_update_spinner_dpi_value(GtkSpinButton *self, GtkRange *range_dpi_value) {
    double value = gtk_range_get_value(range_dpi_value) * 100;

    g_signal_handlers_block_by_func(self, G_CALLBACK(dpi_profile_config_update_range_dpi_value), range_dpi_value);
    gtk_spin_button_set_value(self, value);
    g_signal_handlers_unblock_by_func(self, G_CALLBACK(dpi_profile_config_update_range_dpi_value), range_dpi_value);
}

static void dpi_profile_config_class_init(DpiProfileConfigClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = dpi_profile_config_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/dpi-profile-config.ui");

    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, check_button);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, box_range_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, range_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, spinner_dpi_value);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, color_button_dpi_indicator);
    gtk_widget_class_bind_template_child(widget_class, DpiProfileConfig, button_delete_profile);

    gtk_widget_class_bind_template_callback(widget_class, dpi_profile_config_update_spinner_dpi_value);
    gtk_widget_class_bind_template_callback(widget_class, dpi_profile_config_update_range_dpi_value);
}

static void dpi_profile_config_init(DpiProfileConfig *self) {
    g_type_ensure(DPI_TYPE_PROFILE_CONFIG);
    gtk_widget_init_template(GTK_WIDGET(self));

    GtkWidget *button_decrease_value = gtk_widget_get_next_sibling(
        gtk_widget_get_first_child(GTK_WIDGET(self->spinner_dpi_value))
    );
    GtkWidget *button_increase_value = gtk_widget_get_next_sibling(button_decrease_value);

    gtk_widget_set_visible(button_decrease_value, false);
    gtk_widget_set_visible(button_increase_value, false);

    g_signal_connect_swapped(self->range_dpi_value, "value-changed", G_CALLBACK(dpi_profile_config_update_spinner_dpi_value), self->spinner_dpi_value);
}