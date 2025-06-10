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
	SAVE,
	DISCONNECTED,
	CLOSED,
} typedef MOUSE_STATE;

struct mouse_data {
	GMutex *mutex;
	hid_device *dev;
	color_options *led;
	CONNECTION_TYPE type;
	int battery_level;
	MOUSE_STATE state;
} typedef mouse_data;

struct widget_data {
	mouse_data *mouse;
	GtkLabel *label_battery;
	GtkColorChooser *color_chooser;
} typedef widget_data;

int window_update_loop(void* _data) {
	widget_data *data = (widget_data*) _data;
	mouse_data *mouse = data->mouse;
	
	char battery[4];
	sprintf(battery, "%d", mouse->battery_level);
	
	gtk_label_set_text(data->label_battery, battery);
	
	GdkRGBA color = {};
	gtk_color_chooser_get_rgba(data->color_chooser, &color);
	
	mouse->led->red = (byte) (color.red * 255);
	mouse->led->green = (byte) (color.green * 255);
	mouse->led->blue = (byte) (color.blue * 255);
	
	return G_SOURCE_CONTINUE;
}

void save_mouse_settings(GtkWidget *widget, void *mouse) {
	((mouse_data*) mouse)->state = SAVE;
}

void set_color(GtkColorChooser *self, GdkRGBA *color, void *data) {
	mouse_data *mouse = (mouse_data*) data;
	
	mouse->led->red = (byte) (color->red * 255);
	mouse->led->green = (byte) (color->green * 255);
	mouse->led->blue = (byte) (color->blue * 255);
}

void update_brightness(GtkRange *brightness, void *data) {
	((mouse_data*) data)->led->brightness = (int) gtk_range_get_value(brightness);
}

void close_application(GtkWindow *window, void *data) {
	printf("window closed\n");
	gtk_window_close(window);
}

void activate(GtkApplication *app, void *data) {
	mouse_data *mouse = (mouse_data*) data;
	
	GtkBuilder *builder = gtk_builder_new_from_file("ui/window.ui");
	GtkWindow *window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));
	
	g_signal_connect(window, "close-request", G_CALLBACK(close_application), NULL);
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, mouse);
	widget_add_event(builder, "scaleBrightness", "value-changed", update_brightness, mouse);

	widget_data *widget_data = malloc(sizeof(widget_data));
	widget_data->mouse = mouse;
	widget_data->label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));
	widget_data->color_chooser = GTK_COLOR_CHOOSER(GTK_WIDGET(gtk_builder_get_object(builder, "colorChooserLed")));

	g_timeout_add(10, window_update_loop, widget_data);

	gtk_window_set_application(window, app);
	gtk_window_present(window);
}

void update_battery_level(mouse_data *mouse) {
	byte data[PACKET_SIZE] = {};

	mouse_read(mouse->dev, REPORT_BYTE_HEARTBEAT, data);
	mouse->battery_level = data[4];
}

void* mouse_update_loop(void *data) {
	mouse_data *mouse = (mouse_data*) data;
	
	int res;
    int poll_mouse_type = 0;
	
	while (mouse->state != CLOSED) {
		switch (mouse->state) {
		case SAVE:
			res = save_settings(mouse->dev, mouse->led);
			mouse->state = UPDATE;
			break;
		case DISCONNECTED:
			if (mouse->dev != NULL) {
				hid_close(mouse->dev);
				mouse->dev = NULL;
			}
			
			mouse->dev = open_device(&mouse->type);
			if (mouse->dev) mouse->state = UPDATE;
			
			g_usleep(1000 * 2000);
			break;
		case UPDATE:
			/**
			 * If the user plugs in the mouse cable with the wireless dongle plugged in,
			 * the mouse will switch to wired. However, the wireless device is still available,
			 * but not active, so sending packets to it will have no effect. Thus, we check every second
			 * for to see if the expected connection type is different the actual connection type.
			 */
			if (poll_mouse_type == 9) {
				byte data_buffer[PACKET_SIZE] = {};
				mouse_read(mouse->dev, REPORT_BYTE_CONNECTION, data_buffer);
				
				CONNECTION_TYPE expected_connection = mouse->type;
				get_devices(&mouse->type);
				
				if (expected_connection != mouse->type) {
					mouse->state = DISCONNECTED;
					continue;
				}

				update_battery_level(mouse);
			}
			
			res = change_color(mouse->dev, mouse->led);
            poll_mouse_type = (poll_mouse_type + 1) % 10;
			g_usleep(1000 * 100);
			break;
		default:
			break;
		}
		
		if (res < 0) {
			printf("%d\n", res);
			res = 0;
			mouse->state = DISCONNECTED;
		}
	}
	
	hid_close(mouse->dev);
	printf("mouse closed\n");
	return NULL;
}

int main() {
	int res;
	CONNECTION_TYPE connection_type;
	
	GMutex mutex;
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
		.mutex = &mutex
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

	hid_exit();
	return status;
}