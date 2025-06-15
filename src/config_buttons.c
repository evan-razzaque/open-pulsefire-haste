#include <ctype.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"
#include "config_buttons.h"

static void show_test_window(GSimpleAction *action, GVariant *variant, app_data *data) {
	gtk_window_present(data->widgets->test_window);
}

static void apply_button_bindings(hid_device *dev, GMutex *mutex, mouse_bindings *bindings) {
	g_mutex_lock(mutex);
	assign_button(dev, MOUSE_BUTTON_LEFT, bindings->left);
	assign_button(dev, MOUSE_BUTTON_RIGHT, bindings->right);
	assign_button(dev, MOUSE_BUTTON_MIDDLE, bindings->middle);
	assign_button(dev, MOUSE_BUTTON_SIDE_BACK, bindings->back);
	assign_button(dev, MOUSE_BUTTON_SIDE_FORWARD, bindings->forward);
	assign_button(dev, MOUSE_BUTTON_DPI_TOGGLE, bindings->dpi);
	g_mutex_unlock(mutex);
}

void app_config_buttons_init(GtkBuilder *builder, app_data *data) {
	apply_button_bindings(data->mouse->dev, data->mouse->mutex, &data->button_data.bindings);

	data->widgets->test_window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "windowTest")));
	gtk_window_set_application(data->widgets->test_window, data->widgets->app);

    data->widgets->event_key_controller = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "eventKeyController"));
    data->widgets->label_pressed_key = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelPressedKey")));
	gtk_widget_add_controller(GTK_WIDGET(data->widgets->test_window), data->widgets->event_key_controller);
	
	setup_action_menu_buttons(builder, data);
	
	GSimpleAction *action_show_test_window = g_simple_action_new("show-test-window", NULL);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_show_test_window));	
	g_signal_connect(action_show_test_window, "activate", G_CALLBACK(show_test_window), data);

	GSimpleAction *action_change_binding = g_simple_action_new("change-binding", G_VARIANT_TYPE_STRING);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_change_binding));	
	g_signal_connect(action_change_binding, "activate", G_CALLBACK(change_mouse_binding), data);
	
    g_signal_connect(data->widgets->event_key_controller, "key-pressed", G_CALLBACK(key_pressed_display), data);
}

void set_mouse_button(GtkMenuButton *self, GParamSpec *param_spec, app_data *data) {
	if (!gtk_menu_button_get_active(self)) return;
	
	int *button = g_object_get_data(G_OBJECT(self), "button");
	
	data->button_data.button = *button;
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

void setup_action_menu_buttons(GtkBuilder *builder, app_data *data) {
	GMenuModel *menu_model_actions = G_MENU_MODEL(gtk_builder_get_object(builder, "menuModelActions"));

	data->widgets->menu_button_bindings[0] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonLeft")));
	data->widgets->menu_button_bindings[1] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonRight")));
	data->widgets->menu_button_bindings[2] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonMiddle")));
	data->widgets->menu_button_bindings[3] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonBack")));
	data->widgets->menu_button_bindings[4] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonForward")));
	data->widgets->menu_button_bindings[5] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonDPI")));

	for (int i = 0; i <= MOUSE_BUTTON_DPI_TOGGLE; i++) {
		g_signal_connect(data->widgets->menu_button_bindings[i], "notify::active", G_CALLBACK(set_mouse_button), data);
		g_object_set_data(G_OBJECT(data->widgets->menu_button_bindings[i]), "button", &data->button_data.buttons[i]);
		gtk_menu_button_set_menu_model(data->widgets->menu_button_bindings[i], menu_model_actions);
	}
}

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
	GtkLabel *label_pressed_key = data->widgets->label_pressed_key;
	
	char key_press_info[64];
	
	sprintf(key_press_info, "Value (short): 0x%.4x, Modfifier: %d", tolower(data->button_data.keyboard_keys[keyval]), state);
	gtk_label_set_text(label_pressed_key, key_press_info);
	
	return TRUE;
}
