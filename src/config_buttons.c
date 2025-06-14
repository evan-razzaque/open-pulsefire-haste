#include <gtk/gtk.h>

#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"
#include "config_buttons.h"

void app_config_buttons_init(GtkBuilder *builder, app_data *data) {
    data->widgets->event_key_controller = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "eventKeyController"));
    data->widgets->label_pressed_key = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelPressedKey")));
	gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->widgets->event_key_controller);

	store_action_menu_buttons(builder, data);
	set_menu_buttons_model(builder, data->widgets->menu_button_bindings);

	GSimpleAction *action_change_binding = g_simple_action_new("change-binding", G_VARIANT_TYPE_STRING);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_change_binding));	
	g_signal_connect(action_change_binding, "activate", G_CALLBACK(change_mouse_binding), data);
	
    g_signal_connect(data->widgets->event_key_controller, "key-pressed", G_CALLBACK(key_pressed_display), data->widgets->label_pressed_key);
}

void change_mouse_binding(GSimpleAction *action, GVariant *mapping_data, app_data *data) {
	uint64_t size = 65;

	const char *menu_item_value = g_variant_get_string(mapping_data, &size);
	char hex_value[5] = {};
	char action_name[60] = {};
	
	strncpy(hex_value, menu_item_value, 5);
	strncpy(action_name, menu_item_value + 5, 25);
	
	uint16_t action_value = (uint16_t) strtol(hex_value, NULL, 16);
	
	printf("Button: %d, Value: %.4x, Name: %s\n", data->button_data.button, action_value, action_name);

	g_mutex_lock(data->mouse->mutex);
	assign_button(data->mouse->dev, data->button_data.button, action_value);
	g_mutex_unlock(data->mouse->mutex);

	gtk_menu_button_set_label(data->widgets->menu_button_bindings[data->button_data.button], action_name);
}

void set_mouse_button(GtkMenuButton *self, GParamSpec *param_spec, app_data *data) {
	if (!gtk_menu_button_get_active(self)) return;
	
	int *button = (int*) g_object_get_data(G_OBJECT(self), "button");
	data->button_data.button = *button;
}

void set_menu_buttons_model(GtkBuilder *builder, GtkMenuButton *menu_buttons[]) {
	GMenuModel *menu_model_actions = G_MENU_MODEL(gtk_builder_get_object(builder, "menuModelActions"));
	
	for (int i = 0; i <= MOUSE_BUTTON_DPI; i++) {
		gtk_menu_button_set_menu_model(menu_buttons[i], menu_model_actions);
	}
}

void store_action_menu_buttons(GtkBuilder *builder, app_data *data) {
	data->widgets->menu_button_bindings[0] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonLeft")));
	data->widgets->menu_button_bindings[1] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonRight")));
	data->widgets->menu_button_bindings[2] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonMiddle")));
	data->widgets->menu_button_bindings[3] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonBack")));
	data->widgets->menu_button_bindings[4] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonForward")));
	data->widgets->menu_button_bindings[5] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonDPI")));

	for (int i = 0; i <= MOUSE_BUTTON_DPI; i++) {
		g_signal_connect(data->widgets->menu_button_bindings[i], "notify::active", G_CALLBACK(set_mouse_button), data);
		g_object_set_data(G_OBJECT(data->widgets->menu_button_bindings[i]), "button", &data->button_data.buttons[i]);
	}
}

void add_action_values(mouse_action_values *action_values) {
	action_values->disabled[0] = DISABLED;

	action_values->mouse[0] = LEFT_CLICK;
	action_values->mouse[1] = RIGHT_CLICK;
	action_values->mouse[2] = MIDDLE_CLICK;
	action_values->mouse[3] = BACK;
	action_values->mouse[4] = FORWARD;
	action_values->mouse[5] = DPI_SWITCH;

	action_values->media[0] = PLAY_PAUSE;
	action_values->media[1] = STOP;
	action_values->media[2] = PREVIOUS;
	action_values->media[3] = NEXT;
	action_values->media[4] = MUTE;
	action_values->media[5] = VOLUME_DOWN;
	action_values->media[6] = VOLUME_UP;

	action_values->windows_shortcut[0] = TASK_MANAGER;
	action_values->windows_shortcut[1] = SYSTEM_UTILITY;
	action_values->windows_shortcut[2] = SHOW_DESKTOP;
	action_values->windows_shortcut[3] = CYCLE_APPS;
	action_values->windows_shortcut[4] = CLOSE_APPS;
	action_values->windows_shortcut[5] = CUT;
	action_values->windows_shortcut[6] = COPY;
	action_values->windows_shortcut[7] = PASTE;
}

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data) {
	GtkLabel *label_pressed_key = GTK_LABEL(data);
	
	char key_press_info[64];
	
	sprintf(key_press_info, "Value: %u, Code: %u, Modfifier: %d", keyval, keycode, state);
	gtk_label_set_text(label_pressed_key, key_press_info);
	
	return TRUE;
}
