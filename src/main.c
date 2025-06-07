#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>
#include <gtk/gtkapplication.h>

#include "mouse.h"
#include "rgb.h"
#include "buttons.h"

enum MOUSE_STATE {
	UPDATE,
	SAVE,
	CONNECTION,
	EXIT,
} typedef MOUSE_STATE;

struct mouse_data {
	hid_device *dev;
	color_options *led;
	int state;
} typedef mouse_data;

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

void set_color(color_options *color, uint64_t new_color) {
	color->red = new_color >> 24;
	color->green = new_color >> 16;
	color->blue = new_color >> 8;
	color->brightness = new_color;
}

void set_red(GtkApplication *app, void *color) {
	set_color((color_options*) color, 0xFF000064);
}

void set_green(GtkApplication *app, void *color) {
	set_color((color_options*) color, 0x00FF0064);
}

void set_blue(GtkApplication *app, void *color) {
	set_color((color_options*) color, 0x0000FF64);
}

void save_mouse_settings(GtkApplication *app, void *data) {
	mouse_data *mouse = (mouse_data*) data;
	mouse->state = SAVE;
}

void output_connection_status(GtkApplication *app, void *data) {
	mouse_data *mouse = (mouse_data*) data;
	mouse->state = CONNECTION;
}

void close_application(GtkApplication *app, void *data) {
	GtkWindow *window = (GtkWindow*) data;

	printf("window closed\n");
	gtk_window_close(window);
}

void activate(GtkApplication *app, void *data) {
	mouse_data *mouse = (mouse_data*) data;

	GtkBuilder *builder = gtk_builder_new_from_file("ui/window.ui");
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(GTK_WINDOW(window), "close-request", G_CALLBACK(close_application), GTK_WINDOW(window));
	
	widget_add_event(builder, "buttonRed", "clicked", set_red, mouse->led);
	widget_add_event(builder, "buttonGreen", "clicked", set_green, mouse->led);
	widget_add_event(builder, "buttonBlue", "clicked", set_blue, mouse->led);
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, mouse);
	widget_add_event(builder, "buttonConnection", "clicked", output_connection_status, mouse);

	gtk_window_set_application(GTK_WINDOW(window), app);
	gtk_window_present(GTK_WINDOW(window));
}

void* update_leds(void *data) {
	mouse_data *mouse = (mouse_data*) data;

	while (mouse->state != EXIT) {
		if (mouse->state == SAVE) {
			printf("save\n");
			save_settings(mouse->dev, mouse->led);
			mouse->state = UPDATE;
		} else if (mouse->state == CONNECTION) {
			uint8_t data[PACKET_SIZE] = {};
			mouse_read(mouse->dev, REPORT_CONNECTION, data);
			
			printf("Connection Status: ");
			print_data(data);
			mouse->state = UPDATE;
		} else if (mouse->state == UPDATE) {
			change_color(mouse->dev, mouse->led);
			g_usleep(1000 * 100);
		}
	}

	printf("stopped mouse update loop\n");

	return NULL;
}

int main() {
	int res;

	res = hid_init();
	HID_ERROR(res < 0, NULL);

	hid_device *dev = open_device();
	HID_ERROR(!dev, NULL);

	color_options color = {.red = 0xff, .blue = 0xff, .brightness = 0x64};
	
	mouse_data mouse = {.dev = dev, .led = &color};
	
	GtkApplication *app;
	int status;

	printf("%s\n", __FILE__);

	app = gtk_application_new("org.gtk.pulsefire-haste", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &mouse);

	GThread *updateThread = g_thread_new("update_leds", update_leds, &mouse);
	status = g_application_run(G_APPLICATION(app), 0, NULL);

	g_object_unref(app);
	mouse.state = EXIT;

	g_thread_join(updateThread);

	hid_close(dev);
	hid_exit();

	return status;
}