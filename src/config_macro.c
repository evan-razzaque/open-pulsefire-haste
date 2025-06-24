#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdint.h>

#include "device/buttons.h"
#include "mouse_config.h"

static void open_macro_overlay(GSimpleAction *action, GVariant *variant, app_data *data) {
	gtk_overlay_add_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

static void close_macro_overlay(GtkButton *self, app_data *data) {
	gtk_overlay_remove_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

void app_config_macro_init(GtkBuilder *builder, app_data *data) {
    macro_event events[2] = {
		KEYBOARD_EVENT_DOWN(L_SHIFT, 50, 0x04, 0x05, 0x06),
		KEYBOARD_EVENT_UP(50),
	};

	g_mutex_lock(data->mouse->mutex);
	assign_button_macro(data->mouse->dev, MACRO_BINDING_FORWARD, MACRO_REPEAT_MODE_ONCE, events, 2);
	g_mutex_unlock(data->mouse->mutex);

    data->widgets->box_macro = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMacro")));

    GSimpleAction *action_add_macro = g_simple_action_new("add-macro", NULL);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_add_macro));	
	g_signal_connect(action_add_macro, "activate", G_CALLBACK(open_macro_overlay), data);

    widget_add_event(builder, "buttonMacroCancel", "clicked", close_macro_overlay, data);
}