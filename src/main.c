#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>
#include <assert.h>

#include "types.h"
#include "device/mouse.h"
#include "device/rgb.h"
#include "device/buttons.h"
#include "device/sensor.h"
#include "hotplug/hotplug.h"

#include "settings_storage.h"

#include "hid_keyboard_map.h"
#include "mouse_config.h"
#include "config_led.h"
#include "config_buttons.h"
#include "config_macro.h"
#include "config_sensor.h"

#include "templates/stack_menu_button.h"
#include "templates/stack_menu_button_back.h"
#include "templates/mouse_macro_button.h"
#include "../resources/gresources.h"

#include "util.h"

const char* __asan_default_options() {
	return "detect_leaks=1:handle_ioctl=true";
}

const char* __lsan_default_options() {
	return "suppressions=suppress.txt";
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
	save_device_settings(mouse->dev, &data->color_data->mouse_led);
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

	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (data->button_data->bindings[i] >> 8 == MOUSE_ACTION_TYPE_MACRO) {
			assign_macro(data->macro_data->macro_indicies[i], i, data);
			continue;
		}

		data->macro_data->macro_indicies[i] = -1;
		assign_button_action(dev, i, data->button_data->bindings[i]);
	}

	set_polling_rate(dev, data->sensor_data->polling_rate_value);
	save_dpi_settings(dev, &data->sensor_data->dpi_config, data->sensor_data->lift_off_distance);
}

/**
 * @brief Updates the mouse battery level display.
 * 
 * @param battery_data mouse_battery_data instance
 * @return value indicating to leave this in the gtk main loop
 */
int update_battery_display(app_data *data) {
	mouse_data *mouse = data->mouse;

	if (mouse->battery_level < 0) return G_SOURCE_CONTINUE;
	if (mouse->current_battery_level == mouse->battery_level) return G_SOURCE_CONTINUE;

	mouse->current_battery_level = mouse->battery_level;

	char battery[5];
	sprintf(battery, "%d%%", mouse->battery_level);
	
	gtk_label_set_text(data->widgets->label_battery, battery);
	printf("battery updated\n");
	return G_SOURCE_CONTINUE;
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
	
	for (int i = 0; i < data->macro_data->macro_count; i++) {
		free(data->macro_data->macros[i].events);
		free(data->macro_data->macros[i].name);
	}

	free(data->macro_data->macros);
	
	gtk_window_destroy(data->widgets->window);
	gtk_window_destroy(data->button_data->window_keyboard_action);
	
	printf("window closed\n");
}

/**
 * @brief Reads mouse reports and events.
 * 
 * @param data Application wide data structure
 * @return the report type of the report that was read, or -1 on error
 */
int read_mouse_reports(app_data *data) {
	mouse_data *mouse = data->mouse;

	union report_packet_data report_data = {0};
	REPORT_TYPE report_type = mouse_read(mouse->dev, report_data.packet_data);
	
	switch (report_type) {
	case REPORT_TYPE_HARDWARE:
	case REPORT_TYPE_ONBOARD_LED_SETTINGS:
		break;
	case REPORT_TYPE_HEARTBEAT:
		mouse->battery_level = report_data.heartbeat.battery_level;
		break;
	case REPORT_TYPE_CONNECTION: // generic_event.is_awake shares the same offset with connection_status.is_awake
	case REPORT_TYPE_GENERIC_EVENT:
		if (mouse->connection_type & CONNECTION_TYPE_WIRED) break;

		if (!report_data.generic_event.is_awake && mouse->state != IDLE) {
			printf("idle\n");
			mouse->state = IDLE;
			g_idle_add_once((GSourceOnceFunc) show_connection_lost_overlay, data);
		} else if (report_data.generic_event.is_awake && mouse->state == IDLE) {
			printf("not idle\n");
			mouse->state = UPDATE;
			g_idle_add_once((GSourceOnceFunc) remove_connnection_lost_overlay, data);
		}

		if (report_type == REPORT_TYPE_CONNECTION) break;

		if (report_data.generic_event.selected_dpi_profile == data->sensor_data->dpi_config.selected_profile) {
			break;
		}
		
		dpi_profile_selection_args *args = g_new(dpi_profile_selection_args, 1);
		*args = (dpi_profile_selection_args) {
			.data = data,
			.index = report_data.generic_event.selected_dpi_profile,
			.free_func = g_free
		};

		g_idle_add_once((GSourceOnceFunc) update_dpi_profile_selection, args);
		break;
	default:
		break;
	}

	return report_type;
}

/**
 * @brief A function to periodically attempt to connect to the mouse.
 * 
 * @param mouse The mouse_data instance
 */
void reconnect_mouse(app_data *data) {
	while (data->mouse->dev == NULL) {
		printf("Reconnecting...\n");
		data->mouse->dev = open_device(get_active_devices(data->mouse->connection_type));
		sleep_ms(50);
	}

	printf("Connected\n");
	g_idle_add_once((GSourceOnceFunc) load_mouse_settings, data);
}

void mouse_hotplug_callback(bool connected, app_data *data) {
	mouse_data *mouse = data->mouse;
	
	if (connected) {
		reconnect_mouse(data);
		mouse->state = UPDATE;
		g_idle_add_once((GSourceOnceFunc) show_mouse_settings_visibility, data);
		printf("mouse_hotplug_callback: connected\n");
	} else {
		g_idle_add_once((GSourceOnceFunc) hide_mouse_settings_visibility, data);
		printf("mouse_hotplug_callback: disconnected\n");
	}
}

/**
 * @brief Updates the mouse's status and led settings.
 * 
 * @param mouse mouse_data instance
 * @return Unused
 */
void* mouse_update_loop(app_data *data) {	
	mouse_data *mouse = data->mouse;
	color_options *led = &data->color_data->mouse_led;

	const int update_interval_ms = 25;
	const int update_color_interval_ms = 100;
	const int poll_battery_level_interval_ms = 1000;
	const int poll_connection_status_interval_ms = 5000;

	int clock = 0;
	const int clock_reset_ms = poll_connection_status_interval_ms; 
	
	while (mouse->state != CLOSED) {
		int res = 0;

		g_mutex_lock(mouse->mutex);
		switch (mouse->state) {
		case IDLE:
			res = mouse_send_read_request(mouse->dev, REPORT_TYPE_HEARTBEAT);
			if (res >= 0) res = read_mouse_reports(data);
			
			if (res < 0) mouse->state = DISCONNECTED;

			g_mutex_unlock(mouse->mutex);
			sleep_ms(500);
			continue;
		case DISCONNECTED:
			g_mutex_unlock(mouse->mutex);
			sleep_ms(100);
			continue;
		default:
			break;
		}
		
		if (clock % update_color_interval_ms == 0) {
			res = change_color(mouse->dev, led);
		}
		
		if (clock % poll_battery_level_interval_ms == 0 && res >= 0) {
			res = mouse_send_read_request(mouse->dev, REPORT_TYPE_HEARTBEAT);
		}
		
		if (clock % poll_connection_status_interval_ms == 0 && res >= 0) {
			res = mouse_send_read_request(mouse->dev, REPORT_TYPE_CONNECTION);
		}
		
		if (res >= 0) res = read_mouse_reports(data);
		
		if (res < 0) {		
			mouse->state = DISCONNECTED;
		}
		
		clock = (clock + update_interval_ms) % clock_reset_ms;
		g_mutex_unlock(mouse->mutex);
		sleep_ms(update_interval_ms - READ_TIMEOUT);
	}
	
	if (mouse->dev) hid_close(mouse->dev);
	printf("mouse update thread exit\n");
	g_thread_exit(NULL);
	return NULL;
}

/**
 * @brief A function to initialize the GtkBuilder instance and 
 * obtain widgets to store into `data->widgets`.
 *
 * @param data Application wide data structure
 * @return the GtkBuilder instance
 */
GtkBuilder* init_builder(app_data *data) {
	g_type_ensure(STACK_TYPE_MENU_BUTTON);
	g_type_ensure(STACK_TYPE_MENU_BUTTON_BACK);
	g_type_ensure(MOUSE_TYPE_MACRO_BUTTON);

	g_resources_register(gresources_get_resource());

	GtkBuilder *builder = gtk_builder_new_from_resource("/org/haste/window.ui");
	data->widgets->window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));

	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(provider, "/org/haste/window.css");
	gtk_style_context_add_provider_for_display(
		gtk_widget_get_display(GTK_WIDGET(data->widgets->window)),
		GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
	);

	data->widgets->stack_main = GTK_STACK(GTK_WIDGET(gtk_builder_get_object(builder, "stackMain")));
	data->widgets->box_main = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMain")));
	data->widgets->overlay_main = GTK_OVERLAY(GTK_WIDGET(gtk_builder_get_object(builder, "overlayMain")));
	data->widgets->box_connection_lost = GTK_WIDGET(gtk_builder_get_object(builder, "boxConnectionLost"));

	data->widgets->label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));

	return builder;
}

// TODO: Create application.c
/**
 * @brief A function to setup and activate the application.
 * 
 * @param app GtkApplication instance
 * @param data Application wide data structure
 */
void activate(GtkApplication *app, app_data *data) {
	GtkSettings *settings = gtk_settings_get_default();
	g_object_set(settings, 
		"gtk-application-prefer-dark-theme", true, 
		"gtk-enable-animations", 0,
		"gtk-theme-name", "Adwaita",
		NULL
	);

	GtkBuilder *builder = init_builder(data);
	data->widgets->builder = builder;
	
	app_config_led_init(builder, data);
	app_config_buttons_init(builder, data);
	app_config_macro_init(builder, data);
	app_config_sensor_init(builder, data);
	
	g_signal_connect(data->widgets->window, "close-request", G_CALLBACK(close_application), data);
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, data);
	
	g_timeout_add(100, G_SOURCE_FUNC(update_battery_display), data);

	if (data->mouse->dev == NULL) {
		hide_mouse_settings_visibility(data);
	} else {
		load_mouse_settings(data);
	}

	gtk_window_set_application(data->widgets->window, app);
	gtk_window_present(data->widgets->window);
}

/**
 * @brief A function to set various environment variables.
 */
void set_env() {
	// Fixes memory leaks when switching pages with GtkStack
	g_setenv("GSK_RENDERER", "cairo", true);
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
	
	mouse_data mouse = {.mutex = &mutex, .battery_level = -1};
	
	app_widgets widgets = {0};
	
	app_data data = {
		.mouse = &mouse,
		.widgets = &widgets,
		.color_data = &(config_color_data) {0},
		.sensor_data = &(config_sensor_data) {0},
		.button_data = &(config_button_data) {
			.buttons = {0, 1, 2, 3, 4, 5},
			.default_bindings = {LEFT_CLICK, RIGHT_CLICK, MIDDLE_CLICK, BACK, FORWARD, DPI_TOGGLE},
			.keyboard_keys = KEYBOARD_MAP(),
			.key_names = KEY_NAMES()
		},
		.macro_data = &(config_macro_data) {
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

	mouse_hotplug_data hotplug_data = {
		.mouse = &mouse,
		.hotplug_callback = (hotplug_listener_callback) mouse_hotplug_callback,
		.hotplug_callback_user_data = &data
	};
	
	if (load_settings_from_file(&data) < 0) return -1;
	if (load_macros_from_file(&data) < 0) return -1;

	struct hid_device_info *dev_list = get_devices(&mouse.connection_type);
	
	hotplug_listener_init(&hotplug_data);

	#ifdef _WIN32
		setup_mouse_removal_callbacks(&hotplug_data, dev_list);
	#endif

	mouse.dev = open_device(dev_list);
	
	GThread *update_thread = g_thread_new("mouse_update_loop", (GThreadFunc) mouse_update_loop, &data);
	
	GtkApplication *app;
	int status;
	
	set_env();
	app = gtk_application_new("org.gtk.pulsefire-haste", G_APPLICATION_DEFAULT_FLAGS);
	widgets.app = app;
	
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
