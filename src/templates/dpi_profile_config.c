#include <gtk/gtk.h>

#include "dpi_profile_config.h"

struct _DpiProfileConfig {
    GtkListBoxRow parent_instance;
    
    GtkCheckButton *check_button;
    GtkBox *box_range_dpi_value;
    GtkRange *range_dpi_value;
    GtkGestureClick *gesture_click_controller;
    GtkSpinButton *spinner_dpi_value;
    GtkColorDialogButton *color_button_dpi_indicator;
    GtkButton *button_delete_profile;
    
    bool is_spinner_blocked;
    uint8_t profile_index;
};

G_DEFINE_TYPE(DpiProfileConfig, dpi_profile_config, GTK_TYPE_LIST_BOX_ROW)

enum {
    PROFILE_UPDATED,
    N_SIGNALS
};

static guint signals[N_SIGNALS] = {0};

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

static void dpi_profile_config_update_profile(DpiProfileConfig *self) {
    if (self->is_spinner_blocked) {
        g_signal_handlers_unblock_by_func(
            self->spinner_dpi_value,
            G_CALLBACK(dpi_profile_config_update_profile),
            self
        );
        
        self->is_spinner_blocked = false;
    }

    g_signal_emit(
        self, 
        signals[PROFILE_UPDATED], 
        0,
        self->profile_index,
        gtk_spin_button_get_value_as_int(self->spinner_dpi_value),
        gtk_color_dialog_button_get_rgba(self->color_button_dpi_indicator)
    );
}

static void block_dpi_spinner_update_signal(DpiProfileConfig *self) {
    g_signal_handlers_block_by_func(
        self->spinner_dpi_value,
        G_CALLBACK(dpi_profile_config_update_profile),
        self
    );

    self->is_spinner_blocked = true;
}

static void dpi_profile_config_class_init(DpiProfileConfigClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = dpi_profile_config_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/dpi-profile-config.ui");
    
    signals[PROFILE_UPDATED] = g_signal_new(
        "profile-updated",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE,
        3, G_TYPE_UCHAR, G_TYPE_INT, G_TYPE_POINTER
    );
    
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

    self->is_spinner_blocked = false;

    self->gesture_click_controller = GTK_GESTURE_CLICK(gtk_gesture_click_new());
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(self->gesture_click_controller), GTK_PHASE_CAPTURE);
    gtk_widget_add_controller(
        GTK_WIDGET(self->box_range_dpi_value),
        GTK_EVENT_CONTROLLER(self->gesture_click_controller)
    );

    GtkWidget *button_decrease_value = gtk_widget_get_next_sibling(
        gtk_widget_get_first_child(GTK_WIDGET(self->spinner_dpi_value))
    );
    GtkWidget *button_increase_value = gtk_widget_get_next_sibling(button_decrease_value);
    
    gtk_widget_set_visible(button_decrease_value, false);
    gtk_widget_set_visible(button_increase_value, false);
    
    g_signal_connect_swapped(self->range_dpi_value, "value-changed", G_CALLBACK(dpi_profile_config_update_spinner_dpi_value), self->spinner_dpi_value);

    g_signal_connect_swapped(self->gesture_click_controller, "pressed", G_CALLBACK(block_dpi_spinner_update_signal), self);
    g_signal_connect_swapped(self->gesture_click_controller, "released", G_CALLBACK(dpi_profile_config_update_profile), self);
    g_signal_connect_swapped(self->spinner_dpi_value, "value-changed", G_CALLBACK(dpi_profile_config_update_profile), self);
    g_signal_connect_swapped(self->color_button_dpi_indicator, "notify::rgba", G_CALLBACK(dpi_profile_config_update_profile), self);
}

DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group, uint8_t profile_index) {
    DpiProfileConfig* self = g_object_new(DPI_TYPE_PROFILE_CONFIG, NULL);
    dpi_profile_config_set_index(self, profile_index);
    gtk_check_button_set_group(self->check_button, check_button_group);

    GdkRGBA rgba_indicator = {
        .red   = ((profile_index + 1) & (1 << 0)),
        .green = ((profile_index + 1) & (1 << 1)),
        .blue  = ((profile_index + 1) & (1 << 2)),
        .alpha = 1
    };

    gtk_spin_button_set_value(self->spinner_dpi_value, (1 << profile_index) * 400);
    gtk_color_dialog_button_set_rgba(self->color_button_dpi_indicator, &rgba_indicator);
    
    return self;
}

uint16_t dpi_profile_config_get_dpi_value(DpiProfileConfig *self) {
    return (uint16_t) gtk_spin_button_get_value_as_int(self->spinner_dpi_value);
}

void dpi_profile_config_set_dpi_value(DpiProfileConfig *self, uint16_t dpi_value) {
    gtk_spin_button_set_value(self->spinner_dpi_value, (double) dpi_value);
}

const GdkRGBA* dpi_profile_config_get_indicator(DpiProfileConfig *self) {
    return gtk_color_dialog_button_get_rgba(self->color_button_dpi_indicator);
}

void dpi_profile_config_set_indicator(DpiProfileConfig *self, GdkRGBA *rgb) {
    rgb->alpha = 1;
    gtk_color_dialog_button_set_rgba(self->color_button_dpi_indicator, rgb);
}

void dpi_profile_config_set_index(DpiProfileConfig *self, uint8_t profile_index) {
    self->profile_index = profile_index;
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self), (const char*) G_VARIANT_TYPE_BYTE, profile_index);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->check_button), (const char*) G_VARIANT_TYPE_BYTE, profile_index);
}

void dpi_profile_config_activate(DpiProfileConfig *self) {
    g_signal_emit_by_name(self->check_button, "activate", NULL);
}

void dpi_profile_config_delete_button_set_enabled(DpiProfileConfig *self, bool enabled) {
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_delete_profile), enabled);
}

void dpi_profile_config_remove_check_button_group(DpiProfileConfig *self) {
    gtk_check_button_set_group(self->check_button, NULL);
}

