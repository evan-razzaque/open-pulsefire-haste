#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>
#include <gtk/gtkapplication.h>

#include "types.h"
#include "device/mouse.h"
#include "device/rgb.h"
#include "device/buttons.h"

#include "mouse_led.h"
#include "mouse_buttons.h"

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

void save_mouse_settings(GtkWidget *self, void *_mouse) {
	g_mutex_lock(&mutex);
	
	mouse_data *mouse = _mouse;
	save_settings(mouse->dev, mouse->led);

	g_mutex_unlock(&mutex);
}

int update_battery_display(void *_data) {
	mouse_battery_data *data = _data;
	char battery[5];

	g_mutex_lock(&mutex);
	sprintf(battery, "%d%%", get_battery_level(data->mouse->dev));
	g_mutex_unlock(&mutex);

	gtk_label_set_text(data->label_battery, battery);
	
	return G_SOURCE_CONTINUE;
}

void close_application(GtkWindow *window, void *data) {
	printf("window closed\n");
	gtk_window_close(window);
	gtk_window_destroy(window);
}

void activate(GtkApplication *app, void *_data) {
	app_data *data = _data;
	mouse_data *mouse = data->mouse;
	
	GtkBuilder *builder = gtk_builder_new_from_file("ui/window.ui");
	GtkWindow *window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));
	GtkLabel *label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));

	data->widgets->window = window;
	
	app_mouse_led_init(builder, data);
	app_mouse_buttons_init(builder, data);

	mouse_battery_data *battery_data =  malloc(sizeof(mouse_battery_data));
	*battery_data = (mouse_battery_data) {.mouse = mouse, .label_battery = label_battery};
	
	g_signal_connect(window, "close-request", G_CALLBACK(close_application), NULL);
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, mouse);

	g_timeout_add(2000, update_battery_display, battery_data);

	gtk_window_set_application(window, app);
	gtk_window_present(window);
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
	mouse_data mouse = {.dev = dev, .led = &color, .type = connection_type};

	app_widgets widgets = {};

	app_data data = {.mouse = &mouse, .widgets = &widgets};
	
	GtkApplication *app;
	int status;
	
	app = gtk_application_new("org.gtk.pulsefire-haste", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &data);

	GThread *update_thread = g_thread_new("mouse_update_loop", mouse_update_loop, &mouse);
	status = g_application_run(G_APPLICATION(app), 0, NULL);

	g_object_unref(app);
	mouse.state = CLOSED;

	g_thread_join(update_thread);
	g_thread_unref(update_thread);

	hid_exit();
	return status;
}