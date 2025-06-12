#include <gtk/gtk.h>

#include "types.h"
#include "mouse_buttons.h"

void app_mouse_buttons_init(GtkBuilder *builder, app_data *data) {
    data->widgets->event_key_controller = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "eventKeyController"));
    data->widgets->label_pressed_key = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelPressedKey")));
	gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->widgets->event_key_controller);
	
    g_signal_connect(data->widgets->event_key_controller, "key-pressed", G_CALLBACK(key_pressed_display), data->widgets->label_pressed_key);
}

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data) {
	GtkLabel *label_pressed_key = GTK_LABEL(data);

	char key_press_info[64];

	sprintf(key_press_info, "Value: %u, Code: %u, Modfifier: %d", keyval, keycode, state);
	gtk_label_set_text(label_pressed_key, key_press_info);

	return TRUE;
}