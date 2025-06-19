#include <ctype.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"
#include "mouse_config.h"

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data)\
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

/**
 * @brief Re-binds a mouse button.
 * 
 * @param mouse The mouse_data instance
 * @param button The button to rebind
 * @param action The action to bind to
 * @param menu_button_active The menu button corresponding to the button being re-binded
 * @param action_name The name of the action for updating the label of mouse button menu button
 */
static void change_mouse_binding(mouse_data *mouse, MOUSE_BUTTON button, uint16_t action, GtkMenuButton *menu_button_active, const char *action_name) {
	g_mutex_lock(mouse->mutex);
	assign_button(mouse->dev, button, action);
	g_mutex_unlock(mouse->mutex);

	gtk_menu_button_set_label(menu_button_active, action_name);
}

/**
 * @brief Presents the window for re-binding a button to a keyboard action.
 * 
 * @param action Unused
 * @param variant Unused
 * @param data Application wide data structure
 */
static void show_keyboard_actions_window(GSimpleAction *action, GVariant *variant, app_data *data) {
	gtk_label_set_text(data->widgets->label_selected_button, data->button_data.selected_button_name);
	gtk_window_present(data->widgets->window_keyboard_action);
}

/**
 * @brief Clears the label that displays the key pressed when re-binding a button.
 * 
 * @param self Unused
 * @param label_pressed_key The pressed key label instance
 */
static void clear_key_pressed_label(GtkWindow *self, GtkLabel *label_pressed_key) {
	gtk_label_set_label(label_pressed_key, "");
}

/**
 * @brief Closes the window responsible for re-binding the button to a keyboard action.
 * 
 * @param self Unused
 * @param data Application wide data structure
 */
static void close_keyboard_actions_window(GtkButton *self, app_data *data) {
	gtk_window_close(data->widgets->window_keyboard_action);
}

/**
 * @brief Cofirms the keyboard action for the button being rebinded.
 * 
 * @param self Unused
 * @param data Application wide data structure
 */
static void confirm_keyboard_action_binding(GtkButton *self, app_data *data) {
	byte hid_usage_id = data->button_data.current_keyboard_action;

	change_mouse_binding(
		data->mouse,
		data->button_data.selected_button,
		data->button_data.current_keyboard_action,
		data->widgets->menu_button_bindings[data->button_data.selected_button],
		data->button_data.key_names[hid_usage_id]
	);

	gtk_window_close(data->widgets->window_keyboard_action);
}

/**
 * @brief Applies all the bindings to the mouse.
 * 
 * @param dev The mouse device handle
 * @param mutex The mutex to schedule sending data to the device
 * @param bindings The bindings for each mouse button
 */
static void apply_button_bindings(hid_device *dev, GMutex *mutex, uint16_t *bindings) {
	g_mutex_lock(mutex);

	for (int i = 0; i < BUTTON_COUNT; i++) {
		assign_button(dev, i, bindings[i]);
	}

	g_mutex_unlock(mutex);
}

/**
 * @brief Sets the mouse button to be re-assigned.
 * 
 * @param self The menu button object
 * @param param_spec Unused
 * @param data Application wide data structure
 */
static void set_mouse_button(GtkMenuButton *self, GParamSpec *param_spec, app_data *data) {
	if (!gtk_menu_button_get_active(self)) return;
	
	int *button = g_object_get_data(G_OBJECT(self), "button");
	data->button_data.selected_button = *button;

	memmove(data->button_data.selected_button_name, gtk_widget_get_tooltip_text(GTK_WIDGET(self)), 16);
}

/**
 * Remaps a mouse button to a SIMPLE_MOUSE_ACTION (see device/buttons.h).
 * 
 * @param action Unused
 * @param mapping_data GVariant instance containing the activated menu item's target value.
 * The value is a string containing a SIMPLE_MOUSE_ACTION and the name of the action, seperated by a '|'.
 * @param data Application wide data structure
 */
void change_mouse_simple_binding(GSimpleAction *action, GVariant *mapping_data, app_data *data) {
	uint64_t size = 65;

	const char *menu_item_value = g_variant_get_string(mapping_data, &size);
	char hex_value[5] = {};
	char action_name[25] = {};
	
	strncpy(hex_value, menu_item_value, 5);
	strncpy(action_name, menu_item_value + 5, 25);
	
	uint16_t action_value = (uint16_t) strtol(hex_value, NULL, 16);

	change_mouse_binding(
		data->mouse,
		data->button_data.selected_button,
		action_value,
		data->widgets->menu_button_bindings[data->button_data.selected_button], 
		action_name
	);
}

/**
 * @brief Sets the HID usage id for the key that was pressed.
 * 
 * @param self The GTK key event controller
 * @param keyval The value of the key that was pressed. This gets mapped into a HID usage id.
 * @param keycode Unused
 * @param state Unused
 * @param data Application wide data structure
 * @return return value that tells gtk to stop other handlers from being invoked after the key press.
 */
static int set_keyboard_action(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
	GtkLabel *label_pressed_key = data->widgets->label_pressed_key;
	
	printf("hid usage id: %.4x\n", data->button_data.keyboard_keys[tolower(keyval)]);
	printf("keyval: %.4x\n", keyval);
	byte hid_usage_id = data->button_data.keyboard_keys[tolower(keyval)];
	data->button_data.current_keyboard_action = 0x0200 + hid_usage_id;
	
	const char *key_name = data->button_data.key_names[hid_usage_id];
	gtk_label_set_text(label_pressed_key, key_name);
	
	return TRUE;
}

/**
 * @brief Sets up the menu buttons used for re-assigning each mouse button.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
static void setup_action_menu_buttons(GtkBuilder *builder, app_data *data) {
	GMenuModel *menu_model_actions = G_MENU_MODEL(gtk_builder_get_object(builder, "menuModelActions"));
	GtkMenuButton **menu_buttons = data->widgets->menu_button_bindings;

	menu_buttons[0] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonLeft")));
	menu_buttons[1] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonRight")));
	menu_buttons[2] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonMiddle")));
	menu_buttons[3] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonBack")));
	menu_buttons[4] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonForward")));
	menu_buttons[5] = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonDPI")));

	for (int i = 0; i < BUTTON_COUNT; i++) {
		g_signal_connect(menu_buttons[i], "notify::active", G_CALLBACK(set_mouse_button), data);
		g_object_set_data(G_OBJECT(menu_buttons[i]), "button", &data->button_data.buttons[i]);
		gtk_menu_button_set_menu_model(menu_buttons[i], menu_model_actions);
		gtk_widget_set_hexpand(GTK_WIDGET(menu_buttons[i]), 1);
	}
}

void app_config_buttons_init(GtkBuilder *builder, app_data *data) {
	apply_button_bindings(data->mouse->dev, data->mouse->mutex, data->button_data.bindings);

	data->widgets->window_keyboard_action = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "windowTest")));
    data->widgets->event_key_controller = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "eventKeyController"));
	data->widgets->label_selected_button = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelSelectedButton")));
    data->widgets->label_pressed_key = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelPressedKey")));

	gtk_window_set_application(data->widgets->window_keyboard_action, data->widgets->app);
	gtk_widget_add_controller(GTK_WIDGET(data->widgets->window_keyboard_action), data->widgets->event_key_controller);
	g_signal_connect(data->widgets->window_keyboard_action, "close-request", G_CALLBACK(clear_key_pressed_label), data->widgets->label_pressed_key);
    g_signal_connect(data->widgets->event_key_controller, "key-pressed", G_CALLBACK(set_keyboard_action), data);
	
	setup_action_menu_buttons(builder, data);
	
	GSimpleAction *action_show_test_window = g_simple_action_new("show-test-window", NULL);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_show_test_window));	
	g_signal_connect(action_show_test_window, "activate", G_CALLBACK(show_keyboard_actions_window), data);

	GSimpleAction *action_change_binding = g_simple_action_new("change-binding", G_VARIANT_TYPE_STRING);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_change_binding));	
	g_signal_connect(action_change_binding, "activate", G_CALLBACK(change_mouse_simple_binding), data);
	
	widget_add_event(builder, "buttonKeybindConfirm", "clicked", confirm_keyboard_action_binding, data);
	widget_add_event(builder, "buttonKeybindCancel", "clicked", close_keyboard_actions_window, data);
}