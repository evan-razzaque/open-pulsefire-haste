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
#include "config_led.h"
#include "config_buttons.h"
#include "config_macro.h"
#include "config_sensor.h"

#include "application.h"

#include "util.h"

const char* __asan_default_options() {
	return "detect_leaks=1:handle_ioctl=true";
}

const char* __lsan_default_options() {
	return "suppressions=suppress.txt";
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

			g_idle_add_once((GSourceOnceFunc) load_mouse_settings, data);
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
			goto poll_connection;
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
		
		poll_connection:

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
			.keyboard_keys = KEYBOARD_MAP,
			.key_names = KEY_NAMES,
			.simple_action_names = SIMPLE_ACTION_NAMES
		},
		.macro_data = &(config_macro_data) {
			.modifier_map = MACRO_MODIFIER_MAP,
			.mouse_buttons = MOUSE_MAP,
			.repeat_mode_map = {
				REPEAT_MODE_PLAY_ONCE,
				REPEAT_MODE_TOGGLE_REPEAT,
				REPEAT_MODE_HOLD_REPEAT
			},
			.mouse_button_names = MOUSE_BUTTON_NAMES,
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
