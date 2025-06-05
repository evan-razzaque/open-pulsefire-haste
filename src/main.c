#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk-4.0/gtk/gtk.h>
#include <gtk/gtkapplication.h>

#include "mouse.h"
#include "rgb.h"
#include "buttons.h"

/**
 * Used for debugging purposes.
 * Logs the last error caused by HIDAPI and exits the program.
 * 
 * @param cond The condition to detect the error
 * @param dev The device to close if not NULL
 */
#define HID_ERROR(cond, dev)\
	if ((cond)) {\
		printf("Error: %S\n", hid_error(NULL));\
		if ((dev)) hid_close((dev));\
		hid_exit();\
		return 1;\
	}

void print_hello(GtkApplication *app, gpointer data) {
	g_print("Hello, world\n");
}

void activate(GtkApplication *app, gpointer data) {
	GtkWidget *window;
	GtkWidget *button;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Hello");

	button = gtk_button_new_with_label("Hello, world");
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
	gtk_window_set_child(GTK_WINDOW(window), button);
	gtk_window_present(GTK_WINDOW(window));
}

int main() {
	// int res;

	// res = hid_init();
	// HID_ERROR(res < 0, NULL);

	// hid_device *dev = open_device();
	// HID_ERROR(!dev, NULL);
	
	// color_options options = {.red = 0xff, .blue = 0xff, .brightness = 100};
	// res = change_color(dev, &options);
	// HID_ERROR(res < 0, dev);

	// hid_close(dev);
	// hid_exit();

	GtkApplication *app;
	int status;

	app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), 0, NULL);

	return status;
}
