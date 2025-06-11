#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>
#include <gtk/gtkapplication.h>

#include "device/mouse.h"
#include "device/rgb.h"
#include "device/buttons.h"

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

enum MOUSE_STATE {
	UPDATE,
	CLOSED
} typedef MOUSE_STATE;

struct mouse_data {
	hid_device *dev;
	color_options *led;
	CONNECTION_TYPE type;
	int battery_level;
	MOUSE_STATE state;
} typedef mouse_data;

GMutex mutex;

void save_mouse_settings(GtkWidget *self, void *_mouse) {
	g_mutex_lock(&mutex);
	
	mouse_data *mouse = _mouse;
	save_settings(mouse->dev, mouse->led);

	g_mutex_unlock(&mutex);
}

struct mouse_battery_data {
	mouse_data *mouse;
	GtkLabel *label_battery;
} typedef mouse_battery_data;

int update_battery_display(void *_data) {
	mouse_battery_data *data = _data;
	char battery[5];

	g_mutex_lock(&mutex);
	sprintf(battery, "%d%%", get_battery_level(data->mouse->dev));
	g_mutex_unlock(&mutex);

	gtk_label_set_text(data->label_battery, battery);
	
	return G_SOURCE_CONTINUE;
}

struct mouse_color_data {
	mouse_data *mouse;
	GtkColorChooser *colorChooser;
} typedef mouse_color_data;

int update_color(void *_data) {
	mouse_color_data *data = _data;
	mouse_data *mouse = data->mouse;
	
	GdkRGBA color = {};
	gtk_color_chooser_get_rgba(data->colorChooser, &color);

	mouse->led->red = (byte) (color.red * 255);
	mouse->led->green = (byte) (color.green * 255);
	mouse->led->blue = (byte) (color.blue * 255);

	return G_SOURCE_CONTINUE;
}

void update_brightness(GtkRange *brightness, void *data) {
	((mouse_data*) data)->led->brightness = (int) gtk_range_get_value(brightness);
}

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data) {
	GtkLabel *label_pressed_key = GTK_LABEL(data);

	char key_press_info[64];

	sprintf(key_press_info, "Value: %u, Code: %u, Modfifier: %d", keyval, keycode, state);
	gtk_label_set_text(label_pressed_key, key_press_info);

	return TRUE;
}

void close_application(GtkWindow *window, void *data) {
	printf("window closed\n");
	gtk_window_close(window);
}

void activate(GtkApplication *app, void *data) {
	mouse_data *mouse = (mouse_data*) data;
	
	GtkBuilder *builder = gtk_builder_new_from_file("ui/window.ui");
	GtkWindow *window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));
	GtkLabel *label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));
	GtkLabel *label_pressed_key = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelPressedKey")));
	GtkColorChooser *color_chooser = GTK_COLOR_CHOOSER(GTK_WIDGET(gtk_builder_get_object(builder, "colorChooserLed")));
	
	GtkEventController *event_key_controller = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "eventKeyController"));
	gtk_widget_add_controller(GTK_WIDGET(window), event_key_controller);

	mouse_color_data *color_data = malloc(sizeof(mouse_color_data));
	mouse_battery_data *battery_data =  malloc(sizeof(mouse_battery_data));

	*color_data = (mouse_color_data) {.mouse = mouse, color_chooser = color_chooser};
	*battery_data = (mouse_battery_data) {.mouse = mouse, .label_battery = label_battery};

	g_signal_connect(event_key_controller, "key-pressed", G_CALLBACK(key_pressed_display), label_pressed_key);
	g_signal_connect(window, "close-request", G_CALLBACK(close_application), NULL);

	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, mouse);
	widget_add_event(builder, "scaleBrightness", "value-changed", update_brightness, mouse);

	g_timeout_add(10, update_color, color_data);
	g_timeout_add(2000, update_battery_display, battery_data);

	gtk_window_set_application(window, app);
	gtk_window_present(window);
}

void update_battery_level(mouse_data *mouse) {
	byte data[PACKET_SIZE] = {};

	mouse_read(mouse->dev, REPORT_BYTE_HEARTBEAT, data);
	mouse->battery_level = data[4];
}

void reconnect_mouse(mouse_data *mouse) {
	hid_close(mouse->dev);
	mouse->dev = NULL;

	while (mouse->dev == NULL) {
		g_usleep(1000 * 2000);
		mouse->dev = open_device(NULL);
	}
	
	return;
}

void* mouse_update_loop(void *data) {
	mouse_data *mouse = (mouse_data*) data;
	
	int res;
    int poll_mouse_type = 0;
	
	while (mouse->state != CLOSED) {	
		g_mutex_lock(&mutex);
		
		res = change_color(mouse->dev, mouse->led);
		poll_mouse_type = (poll_mouse_type + 1) % 10;
		
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

int main() {
	int res;
	CONNECTION_TYPE connection_type;
	
	g_mutex_init(&mutex);
	
	res = hid_init();
	HID_ERROR(res < 0, NULL);
	
	hid_device *dev = open_device(&connection_type);
	HID_ERROR(!dev, NULL);

	color_options color = {.red = 0xff, .blue = 0xff, .brightness = 0x64};
	
	mouse_data mouse = {
		.dev = dev,
		.led = &color,
		.type = connection_type,
	};
	
	GtkApplication *app;
	int status;
	
	app = gtk_application_new("org.gtk.pulsefire-haste", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &mouse);

	GThread *update_thread = g_thread_new("mouse_update_loop", mouse_update_loop, &mouse);
	status = g_application_run(G_APPLICATION(app), 0, NULL);

	g_object_unref(app);
	mouse.state = CLOSED;

	g_thread_join(update_thread);
	g_thread_unref(update_thread);

	hid_exit();
	return status;
}