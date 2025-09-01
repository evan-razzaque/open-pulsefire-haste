/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#include "application.h"

#include "config_led.h"
#include "config_buttons.h"
#include "config_macro.h"
#include "config_sensor.h"

#include "templates/stack_menu_button.h"
#include "templates/stack_menu_button_back.h"
#include "templates/mouse_macro_button.h"
#include "templates/mouse_profile_button.h"
#include "../resources/gresources.h"
#include "mouse_profile_storage.h"

#include "util.h"

void show_connection_lost_overlay(app_data *data) {
    GtkStack *stack_main = data->widgets->stack_main;
    GtkBox *box_main = data->widgets->box_main;
    GtkOverlay *overlay_main = data->widgets->overlay_main;

    gtk_stack_set_page(stack_main, STACK_PAGE_MAIN);
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), false);
    gtk_overlay_add_overlay(overlay_main, data->widgets->box_connection_lost);
}

void remove_connnection_lost_overlay(app_data *data) {
    GtkBox *box_main = data->widgets->box_main;
    GtkOverlay *overlay_main = data->widgets->overlay_main;
    GtkWidget *box_connection_lost = data->widgets->box_connection_lost;

    if (data->mouse->state != DISCONNECTED) gtk_widget_set_sensitive(GTK_WIDGET(box_main), true);
    if (gtk_widget_get_parent(box_connection_lost) == NULL) return;

    gtk_overlay_remove_overlay(overlay_main, box_connection_lost);
}

/**
 * @brief A function that will enable the mouse settings screen
 * when the mouse is plugged in and disable it when the mouse is unplugged.
 * 
 * @param data Application wide data structure
 * @return value indicating to leave this in the gtk main loop
 */
static void toggle_mouse_settings_visibility(app_data *data, bool show_mouse_settings) {
    if (show_mouse_settings) {
        gtk_stack_set_page(
            data->widgets->stack_main,
            STACK_PAGE_MAIN
        );
    } else {
        gtk_stack_set_page(
            data->widgets->stack_main,
            STACK_PAGE_DEVICE_NOT_FOUND
        );
    }

    gtk_widget_set_sensitive(
        GTK_WIDGET(data->widgets->box_main),
        show_mouse_settings
    );
}

void hide_mouse_settings_visibility(app_data *data) {
    remove_connnection_lost_overlay(data);
    toggle_mouse_settings_visibility(data, false);
}

void show_mouse_settings_visibility(app_data *data) {
    remove_connnection_lost_overlay(data);
    toggle_mouse_settings_visibility(data, true);
}

/**
 * @brief Updates the mouse battery level display.
 * 
 * @param battery_data mouse_battery_data instance
 * @return value indicating to leave this in the gtk main loop
 */
static int update_battery_display(app_data *data) {
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

void update_menu_button_label(MOUSE_BUTTON button, uint16_t action, app_data *data) {
	GtkMenuButton *menu_button = data->button_data->menu_button_bindings[button];

	byte action_type = action >> 8;
	byte action_value = action & 0x00ff;

	if (action_type == MOUSE_ACTION_TYPE_KEYBOARD) {
		gtk_menu_button_set_label(menu_button, data->button_data->key_names[action_value]);
	} else if (action_type == MOUSE_ACTION_TYPE_MACRO) {
		int macro_index = data->profile->macro_indices[button];
		recorded_macro macro = data->profile->macros[macro_index];

		gtk_menu_button_set_label(menu_button, macro.name);
	} else {
		gtk_menu_button_set_label(menu_button, data->button_data->simple_action_names[action_type][action_value]);
	}
}

void load_mouse_settings(app_data *data) {
	mouse_data *mouse = data->mouse;
	hid_device *dev = mouse->dev;

	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (data->profile->bindings[i] >> 8 == MOUSE_ACTION_TYPE_MACRO) {
			assign_macro(data->profile->macro_indices[i], i, data);
			continue;
		}

		data->profile->macro_indices[i] = -1;
		assign_button(i, data->profile->bindings[i], data);
	}

	g_mutex_lock(mouse->mutex);
	set_polling_rate(dev, data->profile->polling_rate_value);
	g_mutex_unlock(mouse->mutex);

	GVariant *variant_polling_rate = g_variant_new_byte(data->profile->polling_rate_value);
	GVariant *variant_lift_off_distance = g_variant_new_byte(data->profile->lift_off_distance);
	GVariant *variant_selected_dpi_profile = g_variant_new_byte(data->profile->dpi_config.selected_profile);

	g_action_group_activate_action(G_ACTION_GROUP(data->widgets->app), CHANGE_POLLING_RATE, variant_polling_rate);
	g_action_group_activate_action(G_ACTION_GROUP(data->widgets->app), CHANGE_LIFT_OFF_DISTANCE, variant_lift_off_distance);
	g_action_group_activate_action(G_ACTION_GROUP(data->widgets->app), SELECT_DPI_PROFILE, variant_selected_dpi_profile);

	save_dpi_settings(dev, &data->profile->dpi_config, data->profile->lift_off_distance);
}

/**
 * @brief Saves the settings to the mouse.
 * 
 * @param self The save button
 * @param mouse mouse_data instance
 */
void save_mouse_settings(GtkWidget *self, mouse_data *mouse) {
	gtk_widget_set_sensitive(self, false);
	
	g_mutex_lock(mouse->mutex);
	save_device_settings(mouse->dev);
	g_mutex_unlock(mouse->mutex);

	gtk_widget_set_sensitive(self, true);
}

/**
 * A function to save mouse profiles to disk then remove them from `app_data->mouse_profiles`.
 * This is used exclusively with `g_hash_table_foreach_remove` when the application is closed. 
 * See also: `save_profile_to_file`.
 */
static bool remove_saved_mouse_profile_from_hash_map(char *name, mouse_profile *profile, app_data *data) {
	save_profile_to_file(name, profile, data);
	return true;
}

/**
 * @brief Destroys all windows and saves all mouse profiles when the main window is closed.
 * 
 * @param window The main window
 * @param data Application wide data structure
 */
static void close_application(GtkWindow *window, app_data *data) {
	save_profile_to_file(data->profile_name, data->profile, data);
	free(data->app_data_dir);

	g_hash_table_foreach_remove(data->mouse_profiles, (GHRFunc) remove_saved_mouse_profile_from_hash_map, data);
	g_hash_table_destroy(data->mouse_profiles);
	
	gtk_window_destroy(data->widgets->window);
	gtk_window_destroy(data->button_data->window_keyboard_action);

	printf("window closed\n");
}

/**
 * @brief A function to initialize the GtkBuilder instance and 
 * obtain widgets to store into `data->widgets`.
 *
 * @param data Application wide data structure
 * @return the GtkBuilder instance
 */
static GtkBuilder* init_builder(app_data *data) {
	g_type_ensure(STACK_TYPE_MENU_BUTTON);
	g_type_ensure(STACK_TYPE_MENU_BUTTON_BACK);
	g_type_ensure(MOUSE_TYPE_MACRO_BUTTON);
	g_type_ensure(MOUSE_TYPE_PROFILE_BUTTON);

	g_resources_register(gresources_get_resource());

	GtkBuilder *builder = gtk_builder_new_from_resource("/open_pulsefire_haste/window.ui");
	data->widgets->window = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "window")));

	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(provider, "/open_pulsefire_haste/window.css");
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
	widget_add_event(builder, "buttonSave", "clicked", save_mouse_settings, data->mouse);
	
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
 * @brief A function to switch to the main stack page.
 * 
 * @param stack The main stack widget
 * @param controller Unused
 */
G_MODULE_EXPORT void enter_main_stack_page(GtkStack *stack, GtkEventController *controller) {
    gtk_stack_set_page(stack, STACK_PAGE_MAIN);
}

G_MODULE_EXPORT void switch_stack_page(GtkStack *stack, GtkActionable *button) {
    gtk_stack_set_page(
        stack, 
        g_variant_get_int32(gtk_actionable_get_action_target_value(button)
    ));
}

G_MODULE_EXPORT void disable_main_stack_page(GtkBox *box_main, GtkActionable *button) {
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), false);
}

G_MODULE_EXPORT void enable_main_stack_page(GtkBox *box_main, GtkActionable *button) {
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), true);
}