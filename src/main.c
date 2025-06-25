#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "types.h"
#include "device/mouse.h"
#include "device/rgb.h"
#include "device/buttons.h"

#include "hid_keyboard_map.h"
#include "mouse_config.h"

/**
 * Used for debugging purposes.
 * Logs the last error caused by HIDAPI and exits the program.
 * 
 * @param cond The condition to detect the error
 * @param dev The device to close if not NULL
 */
#define HID_ERROR(cond, dev)\
	if ((cond)) {\
		printf("Error: %S\n", hid_error(dev));\
		if ((dev)) hid_close((dev));\
		hid_exit();\
		return 1;\
	}

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data)\
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

GMutex mutex;

/**
 * @brief Saves the settings to the mouse.
 * 
 * @param self The save button
 * @param mouse mouse_data instance
 */
void save_mouse_settings(GtkWidget *self, mouse_data *mouse) {
	gtk_widget_set_sensitive(self, FALSE);
	
	g_mutex_lock(&mutex);
	save_settings(mouse->dev, mouse->led);
	g_mutex_unlock(&mutex);

	gtk_widget_set_sensitive(self, TRUE);
}

/**
 * @brief Updates the mouse battery display.
 * 
 * @param battery_data mouse_battery_data instance
 * @return value indicating to leave this in the gtk main loop
 */
int update_battery_display(mouse_battery_data *battery_data) {
	char battery[5];

	g_mutex_lock(&mutex);

	int res = get_battery_level(battery_data->mouse->dev);
	sprintf(battery, "%d%%", res);
	
	g_mutex_unlock(&mutex);

	if (res > 0) gtk_label_set_text(battery_data->label_battery, battery);
	
	return G_SOURCE_CONTINUE;
}

/**
 * @brief Destroys all windows when the main window is closed.
 * 
 * @param window The main application window
 * @param data Application wide data structure
 */
void close_application(GtkWindow *window, app_data *data) {
	printf("window closed\n");
	gtk_window_destroy(window);
	gtk_window_destroy(data->widgets->window_keyboard_action);
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
		"gtk-application-prefer-dark-theme", TRUE, 
		"gtk-enable-animations", 0,
		"gtk-theme-name", "Adwaita",
		NULL
	);
	
	GtkBuilder *builder = gtk_builder_new_from_file("ui/window.ui");
	GtkWindow *window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));
	GtkLabel *label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));

	data->widgets->window = window;
	
	app_config_led_init(builder, data);
	app_config_buttons_init(builder, data);
	app_config_macro_init(builder, data);

	data->battery_data = (mouse_battery_data) {.mouse = mouse, .label_battery = label_battery};
	
	g_signal_connect(window, "close-request", G_CALLBACK(close_application), data);
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, mouse);

	g_timeout_add(2000, G_SOURCE_FUNC(update_battery_display), &data->battery_data);

	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "ui/window.css");
	gtk_style_context_add_provider_for_display(gtk_widget_get_display(GTK_WIDGET(window)), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	
	gtk_window_set_application(window, app);
	gtk_window_present(window);
}

/**
 * @brief A function to periodically attempt to connect to the mouse when disconnected.
 * 
 * @param mouse mouse_data instance
 */
void reconnect_mouse(mouse_data *mouse) {
	hid_close(mouse->dev);
	mouse->dev = NULL;

	while (mouse->dev == NULL) {
		g_usleep(1000 * 2000);
		mouse->dev = open_device(NULL);
	}
	
	return;
}

/**
 * @brief Updates the mouse led settings, which allows every other setting to be preserved while the application is running.
 * 
 * @param mouse mouse_data instance
 * @return Unused
 */
void* mouse_update_loop(mouse_data *mouse) {	
	int res;
	
	while (mouse->state != CLOSED) {
		g_mutex_lock(&mutex);

		res = change_color(mouse->dev, mouse->led);
		
		if (res < 0) {
			printf("%d\n", res);
			res = 0;
			reconnect_mouse(mouse);
		}
		
		g_mutex_unlock(&mutex);
		g_usleep(1000 * 100);
	}
	
	if (mouse->dev) hid_close(mouse->dev);
	printf("mouse closed\n");
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
	CONNECTION_TYPE connection_type;
	
	g_mutex_init(&mutex);
	
	res = hid_init();
	HID_ERROR(res < 0, NULL);
	
	hid_device *dev = open_device(&connection_type);
	HID_ERROR(!dev, NULL);

	color_options color = {.red = 0xff, .brightness = 0x64};
	mouse_data mouse = {.mutex = &mutex, .dev = dev, .led = &color, .type = connection_type};

	app_widgets widgets = {};

	app_data data = {
		.mouse = &mouse,
		.widgets = &widgets,
		.button_data = {
			.dev = dev,
			.buttons = {0, 1, 2, 3, 4, 5},
			// TODO: Read buttons from file
			.bindings = {
				LEFT_CLICK,
				RIGHT_CLICK,
				MIDDLE_CLICK,
				BACK,
				FORWARD,
				DPI_TOGGLE
			},
			.keyboard_keys = KEYBOARD_MAP(),
			.key_names = KEY_NAMES()
		},
		.macro_data = {
			.modifier_map = MACRO_MODIFIER_MAP(),
			.mouse_buttons = MOUSE_MAP(),
			.mouse_button_names = MOUSE_BUTTON_NAMES(),
			.recording_macro = 0,
			.event_index = 0,
			.macro_count = 0
		}
	};
	
	
	GtkApplication *app;
	int status;
	
	app = gtk_application_new("org.gtk.pulsefire-haste", G_APPLICATION_DEFAULT_FLAGS);
	widgets.app = app;

	g_signal_connect(app, "activate", G_CALLBACK(activate), &data);
	
	GThread *update_thread = g_thread_new("mouse_update_loop", (GThreadFunc) mouse_update_loop, &mouse);
	status = g_application_run(G_APPLICATION(app), 0, NULL);

	g_object_unref(app);
	mouse.state = CLOSED;
	
	g_thread_join(update_thread);
	g_thread_unref(update_thread);
	
	hid_exit();
	return status;
}