#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/mouse.h"
#include "device/rgb.h"
#include "device/buttons.h"
#include "device/sensor.h"
#include "hotplug/hotplug.h"

#include "hid_keyboard_map.h"
#include "mouse_config.h"

#include "templates/stack_menu_button.h"
#include "templates/stack_menu_button_back.h"
#include "templates/mouse_macro_button.h"

#include "templates/gresources.h"

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data)\
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

const char* __asan_default_options() {
	return "detect_leaks=0";
}

/**
 * @brief Saves the settings to the mouse.
 * 
 * @param self The save button
 * @param mouse mouse_data instance
 */
void save_mouse_settings(GtkWidget *self, app_data *data) {
	mouse_data *mouse = data->mouse;

	gtk_widget_set_sensitive(self, false);
	
	g_mutex_lock(mouse->mutex);
	save_device_settings(mouse->dev, &data->color_data.mouse_led);
	g_mutex_unlock(mouse->mutex);

	gtk_widget_set_sensitive(self, true);
}

/**
 * @brief Sets the settings for the mouse.
 * 
 * @param data Application wide data structure
 */
static void load_mouse_settings(app_data *data) {
	hid_device *dev = data->mouse->dev;

	if (dev == NULL) return;

	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (data->button_data.bindings[i] >> 8 == MOUSE_ACTION_TYPE_MACRO) {
			assign_macro(data->macro_data.macro_indicies[i], i, data);
			continue;
		}

		data->macro_data.macro_indicies[i] = -1;
		assign_button_action(dev, i, data->button_data.bindings[i]);
	}

	set_polling_rate(dev, data->sensor_data.polling_rate_value);
	save_dpi_settings(dev, &data->sensor_data.dpi_config, data->sensor_data.lift_off_distance);
}

/**
 * @brief Updates the mouse battery display.
 * 
 * @param battery_data mouse_battery_data instance
 * @return value indicating to leave this in the gtk main loop
 */
/* int update_battery_display(mouse_battery_data *battery_data) {
	char battery[5];

	GMutex *mutex = battery_data->mouse->mutex;

	g_mutex_lock(mutex);

	int res = get_battery_level(battery_data->mouse->dev);
	sprintf(battery, "%d%%", res);
	
	g_mutex_unlock(mutex);

	if (res > 0) gtk_label_set_text(battery_data->label_battery, battery);
	
	return G_SOURCE_CONTINUE;
} */

void unref_widgets(app_data *data) {
	g_object_ref_sink(data->sensor_data.check_button_group_dpi_profile);
	g_object_unref(data->sensor_data.check_button_group_dpi_profile);

	g_object_ref_sink(data->widgets->alert);
	g_object_unref(data->widgets->alert);
}

/**
 * @brief Destroys all windows when the main window is closed.
 * 
 * @param window The main application window
 * @param data Application wide data structure
 */
void close_application(GtkWindow *window, app_data *data) {
	save_settings_to_file(data);
	save_macros_to_file(data);
	
	for (int i = 0; i < data->macro_data.macro_count; i++) {
		free(data->macro_data.macros[i].events);
		free(data->macro_data.macros[i].name);
	}

	free(data->macro_data.macros);

	unref_widgets(data);
	printf("window closed\n");

	gtk_window_destroy(data->widgets->window);
	gtk_window_destroy(data->button_data.window_keyboard_action);
}

/**
 * @brief A function to setup and activate the application.
 * 
 * @param app GtkApplication instance
 * @param data Application wide data structure
 */
void activate(GtkApplication *app, app_data *data) {
	mouse_data *mouse = data->mouse;

	GtkSettings *settings = gtk_settings_get_default();
	g_object_set(settings, 
		"gtk-application-prefer-dark-theme", true, 
		"gtk-enable-animations", 0,
		"gtk-theme-name", "Adwaita",
		NULL
	);

	g_type_ensure(STACK_TYPE_MENU_BUTTON);
	g_type_ensure(STACK_TYPE_MENU_BUTTON_BACK);
	g_type_ensure(MOUSE_TYPE_MACRO_BUTTON);
	
	// g_resources_register(g_resource_load("resources/templates.gresource", NULL));
	g_resources_register(gresources_get_resource());
	
	GtkBuilder *builder = gtk_builder_new_from_resource("/org/haste/window.ui");
	data->widgets->builder = builder;

	GtkWindow *window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));
	GtkLabel *label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));

	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(provider, "/org/haste/window.css");
	gtk_style_context_add_provider_for_display(gtk_widget_get_display(GTK_WIDGET(window)), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	data->widgets->window = window;
	data->widgets->stack_main = GTK_STACK(GTK_WIDGET(gtk_builder_get_object(builder, "stackMain")));
	data->widgets->box_main = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMain")));
	
	app_config_led_init(builder, data);
	app_config_buttons_init(builder, data);
	app_config_macro_init(builder, data);
	app_config_sensor_init(builder, data);

	if (mouse->dev != NULL) {
		load_mouse_settings(data);
	}

	data->battery_data = (mouse_battery_data) {.mouse = mouse, .label_battery = label_battery};

	g_signal_connect(window, "close-request", G_CALLBACK(close_application), data);
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, data);

	g_timeout_add(100, G_SOURCE_FUNC(toggle_mouse_settings_visibility), data);
	
	gtk_window_set_application(window, app);
	gtk_window_present(window);
}

/**
 * @brief A function to periodically attempt to connect to the mouse.
 * 
 * @param mouse The mouse_data instance
 */
void reconnect_mouse(app_data *data) {
	while (data->mouse->dev == NULL) {
		printf("Reconnecting...\n");
		data->mouse->dev = open_device(get_active_devices(data->mouse->type));
		g_usleep(1000 * 50);
	}

	printf("Connected\n");
	data->mouse->state = CONNECTED;
	load_mouse_settings(data);
}

/**
 * @brief Updates the mouse connection and led settings, which in turn saves every other
 * setting while the mouse is actively being updated.
 * 
 * @param mouse mouse_data instance
 * @return Unused
 */
void* mouse_update_loop(app_data *data) {	
	mouse_data *mouse = data->mouse;
	color_options *led = &data->color_data.mouse_led;

	int res;
	
	while (mouse->state != CLOSED) {
		if (mouse->state == RECONNECT) reconnect_mouse(data);
		if (mouse->dev == NULL || mouse->state != UPDATE) {
			g_usleep(1000 * 100);
			continue;
		}

		g_mutex_lock(mouse->mutex);

		res = change_color(mouse->dev, led);
		
		if (res < 0 && mouse->state != RECONNECT) {
			printf("%d\n", res);
			res = 0;
			mouse->dev = NULL;
		}

		g_mutex_unlock(mouse->mutex);
		g_usleep(1000 * 100);
	}
	
	if (mouse->dev) hid_close(mouse->dev);
	printf("mouse update thread exit\n");
	g_thread_exit(NULL);
	return NULL;
}

/**
 * @brief Opens the mouse device handle and mouse update thread, sets up application data, and initializes GTK.
 * 
 * @return exit status 
 */
int main() {
	int res;

	GMutex mutex;
	g_mutex_init(&mutex);

	res = hid_init();
	if (res < 0) return 1;
	
	mouse_data mouse = {.mutex = &mutex};

	mouse_hotplug_data hotplug_data = {0};
	hotplug_listener_init(&hotplug_data, &mouse);
	
	struct hid_device_info *dev_list = get_devices(&mouse.type);
	setup_mouse_removal_callbacks(&hotplug_data, dev_list);
	mouse.dev = open_device(dev_list);
	dev_list = NULL;
	
	app_widgets widgets = {.alert = gtk_alert_dialog_new(" ")};
	
	app_data data = {
		.mouse = &mouse,
		.widgets = &widgets,
		.button_data = {
			.buttons = {0, 1, 2, 3, 4, 5},
			.default_bindings = {LEFT_CLICK, RIGHT_CLICK, MIDDLE_CLICK, BACK, FORWARD, DPI_TOGGLE},
			.keyboard_keys = KEYBOARD_MAP(),
			.key_names = KEY_NAMES()
		},
		.macro_data = {
			.modifier_map = MACRO_MODIFIER_MAP(),
			.mouse_buttons = MOUSE_MAP(),
			.repeat_mode_map = {
				REPEAT_MODE_PLAY_ONCE,
				REPEAT_MODE_TOGGLE_REPEAT,
				REPEAT_MODE_HOLD_REPEAT
			},
			.mouse_button_names = MOUSE_BUTTON_NAMES(),
			.is_recording_macro = false,
			.is_resuming_macro_recording = false
		}
	};

	if (load_settings_from_file(&data) < 0) return -1;
	if (load_macros_from_file(&data) < 0) return -1;
	
	GtkApplication *app;
	int status;
	
	app = gtk_application_new("org.gtk.pulsefire-haste", G_APPLICATION_DEFAULT_FLAGS);
	widgets.app = app;
	
	GThread *update_thread = g_thread_new("mouse_update_loop", (GThreadFunc) mouse_update_loop, &data);
	
	g_signal_connect(app, "activate", G_CALLBACK(activate), &data);
	status = g_application_run(G_APPLICATION(app), 0, NULL);

	g_object_unref(app);
	mouse.state = CLOSED;
	
	g_thread_join(update_thread);
	g_thread_unref(update_thread);
	
	hotplug_listener_exit(&hotplug_data);
	
	hid_exit();
	return status;
}
