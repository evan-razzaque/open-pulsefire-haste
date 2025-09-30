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
#include "templates/recorded_macro_button.h"
#include "templates/mouse_profile_button.h"
#include "../resources/gresources.h"
#include "mouse_profile_storage.h"

#include "defs.h"
#include <assert.h>

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

void load_mouse_profile_to_mouse(app_data *data) {
	mouse_data *mouse = data->mouse;
	hid_device *dev = mouse->dev;

	mouse_profile *profile = data->profile;

	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (profile->bindings[i] >> 8 == MOUSE_ACTION_TYPE_MACRO) {
			assign_macro(profile->macro_indices[i], i, data);
			continue;
		}

		profile->macro_indices[i] = -1;
		assign_button(i, profile->bindings[i], data);
	}

	g_mutex_lock(mouse->mutex);

	create_dpi_profile_rows(&data->profile->dpi_config, data);
	create_macro_entries(data);

	GVariant *variant_polling_rate = g_variant_new_byte(profile->polling_rate_value);
	GVariant *variant_lift_off_distance = g_variant_new_byte(profile->lift_off_distance);
	GVariant *variant_selected_dpi_profile = g_variant_new_byte(profile->dpi_config.selected_profile);

	g_action_group_activate_action(G_ACTION_GROUP(data->widgets->app), CHANGE_POLLING_RATE, variant_polling_rate);
	g_action_group_activate_action(G_ACTION_GROUP(data->widgets->app), CHANGE_LIFT_OFF_DISTANCE, variant_lift_off_distance);
	g_action_group_activate_action(G_ACTION_GROUP(data->widgets->app), SELECT_DPI_PROFILE, variant_selected_dpi_profile);

	if (mouse->is_saving_settings) {
		mouse->outdated_settings[SEND_BYTE_POLLING_RATE & 0x0f] = true;
		mouse->outdated_settings[SEND_BYTE_DPI & 0x0f] = true;
	} else {
		set_polling_rate(dev, profile->polling_rate_value);
		save_dpi_settings(dev, &data->profile->dpi_config, data->profile->lift_off_distance);
	}

	g_mutex_unlock(mouse->mutex);
}

/**
 * @brief Updates the button assignments for the mosue.
 *
 * @param data Application wide data structure
 */
static void update_mouse_button_assignments(app_data *data) {
	mouse_data *mouse = data->mouse;
	mouse_profile *profile = data->profile;

	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (profile->bindings[i] >> 8 == MOUSE_ACTION_TYPE_MACRO) {
			assign_macro_to_mouse(profile->macro_indices[i], i, data);
			continue;
		}

		assign_button_action(mouse->dev, i, profile->bindings[i]);
	}
}

static void save_mouse_settings_timeout(app_data *data) {
	GtkWidget *button_save_mouse_settings = (GtkWidget*) data->widgets->button_save_mouse_settings;
	mouse_data *mouse = data->mouse;

	g_mutex_lock(mouse->mutex);

	mouse->is_saving_settings = false;

	// Update mouse settings during mouse save

	for (int i = 0; i < sizeof(mouse->outdated_settings); i++) {
		int setting_to_update = (i + SEND_BYTE_POLLING_RATE) * mouse->outdated_settings[i];

		switch (setting_to_update) {
		case SEND_BYTE_POLLING_RATE:
			set_polling_rate(mouse->dev, data->profile->polling_rate_value);
			break;
		case SEND_BYTE_DPI:
			save_dpi_settings(mouse->dev, &data->profile->dpi_config, data->profile->lift_off_distance);
			break;
		case SEND_BYTE_BUTTON_ASSIGNMENT:
			update_mouse_button_assignments(data);
			break;
		default:
			break;
		}

		mouse->outdated_settings[i] = false;
	}

	// End update

	g_mutex_unlock(mouse->mutex);

	gtk_widget_set_sensitive(button_save_mouse_settings, true);
}

void save_mouse_settings(GtkWidget *self, app_data *data) {
	gtk_widget_set_sensitive(self, false);

	g_mutex_lock(data->mouse->mutex);

	data->mouse->is_saving_settings = true;
	save_device_settings(data->mouse->dev, &data->profile->led);

	g_mutex_unlock(data->mouse->mutex);

	g_timeout_add_seconds_once(3, (GSourceOnceFunc) save_mouse_settings_timeout, data);
}

/**
 * A function to save mouse profiles to disk then remove them from `app_data->mouse_profiles`.
 *
 * See also: `save_profile_to_file`, `close_application`.
 */
static bool remove_saved_mouse_profile_from_hash_map(char *name, mouse_profile *profile, app_data *data) {
	printf("Removed Profile: %s\n", name);
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
	save_selected_profile_name(data->profile_name);

	g_mutex_lock(data->mouse->mutex);

	data->profile = NULL;

	g_hash_table_foreach_remove(data->mouse_profiles, (GHRFunc) remove_saved_mouse_profile_from_hash_map, data);
	g_hash_table_destroy(data->mouse_profiles);

	gtk_window_destroy(data->widgets->window);
	gtk_window_destroy(data->widgets->window_new_mouse_profile);
	gtk_window_destroy(data->button_data->window_keyboard_action);
	printf("window closed\n");

	data->mouse->state = CLOSED;

	g_mutex_unlock(data->mouse->mutex);
}

/**
 * @brief Switches to the mouse profile `profile_name`.
 *
 * @param mouse_profile_button Unused, can be NULL
 * @param name The name of the profile
 * @param data Application wide data structure
 */
static void switch_mouse_profile(MouseProfileButton *mouse_profile_button, const char *profile_name, app_data *data) {
	if (strcmp(data->profile_name, profile_name) == 0) {
		debug("Already on profile %s\n", profile_name);
		return;
	}

	g_mutex_lock(data->mouse->mutex);
	int res = switch_profile(profile_name, data);
	g_mutex_unlock(data->mouse->mutex);

	if (res < 0) {
		debug("Couldn't switch to profile %s\n", profile_name);
		return;
	}

	g_strlcpy(data->profile_name, profile_name, sizeof(data->profile_name));
	printf("Switched to %s\n", profile_name);

	load_mouse_profile_to_mouse(data);

	gtk_menu_button_set_label(
		data->widgets->menu_button_mouse_profiles,
		profile_name
	);

	gtk_menu_button_popdown(data->widgets->menu_button_mouse_profiles);
}

/**
 * @brief Rename the mouse profile `old_name` to `new_name`.
 *
 * @param self The MouseProfileButton instance
 * @param old_name The current name of the mouse profile
 * @param new_name The new profile name
 * @param data Application wide data structure
 * @return true if the profile was renamed or false if it wasn't
 */
static bool rename_mouse_profile(MouseProfileButton *self, const char *old_name, const char *new_name, app_data *data) {
	int res = rename_profile(old_name, new_name, data);
	if (res < 0) return false;

	debug("old_name = %s, data->profile_name = %s\n", old_name, data->profile_name);
	debug("%d\n", data->profile->polling_rate_value);

	if (strcmp(data->profile_name, old_name) == 0) {
		g_strlcpy(data->profile_name, new_name, PROFILE_NAME_MAX_LENGTH);
		gtk_menu_button_set_label(data->widgets->menu_button_mouse_profiles, new_name);
	}

	return true;
}

/**
 * @brief Deletes a mouse profile
 *
 * @param self The MouseProfileButton instance
 * @param name The name of the mouse profile
 * @param data Application wide data structure
 * @return true if the profile was deleted or false if it wasn't
 */
static bool delete_mouse_profile(MouseProfileButton *self, const char *name, app_data *data) {
	int res = delete_profile(name, data);
	if (res < 0) return false;

	if (strcmp(data->profile_name, name) == 0) {
		switch_mouse_profile(NULL, DEFAULT_PROFILE_NAME, data);
	}

	return true;
}

/**
 * @brief A function to add `MouseProfileButton` to the mouse profile dropdown list.
 *
 * @param profile_name The name of the mouse profile
 * @param data Application wide data structure
 * @param is_default_profile Whether the mouse profile is the default profile or not
 */
static void add_mouse_profile_button(const char *profile_name, app_data *data, bool is_default_profile) {
	MouseProfileButton *profile_button = mouse_profile_button_new(profile_name, is_default_profile);
	g_signal_connect(profile_button, "select-profile", G_CALLBACK(switch_mouse_profile), data);
	g_signal_connect(profile_button, "rename-profile", G_CALLBACK(rename_mouse_profile), data);
	g_signal_connect(profile_button, "delete-profile", G_CALLBACK(delete_mouse_profile), data);

	gtk_box_append(
		data->widgets->box_mouse_profiles,
		GTK_WIDGET(profile_button)
	);
}

/**
 * @brief A function to create a new mouse profile.
 *
 * @param button Unused
 * @param data Application wide data structure
 */
static void create_new_mouse_profile(GtkButton *button, app_data *data) {
	const char *name = gtk_editable_get_text(data->widgets->editable_profile_name);

	if (!is_valid_profile_name(name)) {
		gtk_label_set_text(data->widgets->label_profile_name_error, "Invalid profile name");
		return;
	}

	if (profile_file_exists(name)) {
		gtk_label_set_text(data->widgets->label_profile_name_error, "A profile with this name already exists");
		return;
	}

	gtk_label_set_text(data->widgets->label_profile_name_error, "");

	add_mouse_profile_button(name, data, false);
	switch_mouse_profile(NULL, name, data);

	gtk_window_close(data->widgets->window_new_mouse_profile);
}

/**
 * @brief A method to reset the window used to create a new mouse profile.
 *
 * @param widget Unused
 * @param data Application wide data structure
 */
static void reset_new_profile_window(GtkWidget *widget, app_data *data) {
	debug("close profile window\n");
	gtk_editable_set_text(data->widgets->editable_profile_name, "");
	gtk_label_set_text(data->widgets->label_profile_name_error, "");
}

/**
 * @brief Adds the mouse profiles entries to the profile menu button.
 *
 * @param data Application wide data structure
 */
static void add_mouse_profile_entries(app_data *data) {
	add_mouse_profile_button(DEFAULT_PROFILE_NAME, data, true);
	gtk_menu_button_set_label(data->widgets->menu_button_mouse_profiles, data->profile_name);

	GDir *profiles_dir = g_dir_open(PROFILE_DIR, 0, NULL);
	if (profiles_dir == NULL) {
		debug("Error\n");
	} else {
		const char *profile_filename;

		while ((profile_filename = g_dir_read_name(profiles_dir))) {
			data->profile_count++;

			if (strcmp(profile_filename, DEFAULT_PROFILE_NAME PROFILE_EXTENSION) == 0) continue;

			int profile_name_size = (strlen(profile_filename) + 1) - (PROFILE_EXTENSION_LENGTH + 1);
			char *profile_name = g_strndup(profile_filename, profile_name_size);

			assert(profile_name[profile_name_size] == 0);

			add_mouse_profile_button(profile_name, data, false);
			free(profile_name);
		}
	}

	g_dir_close(profiles_dir);
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
	g_type_ensure(RECORDED_TYPE_MACRO_BUTTON);
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

	data->widgets->label_battery = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelBattery")));
	data->widgets->button_save_mouse_settings = GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "buttonSave")));

	data->widgets->box_mouse_profiles = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMouseProfiles")));
	data->widgets->menu_button_mouse_profiles = GTK_MENU_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "menuButtonMouseProfile")));
	data->widgets->window_new_mouse_profile = GTK_WINDOW(GTK_WIDGET(gtk_builder_get_object(builder, "windowNewMouseProfile")));
	data->widgets->editable_profile_name = GTK_EDITABLE(GTK_WIDGET(gtk_builder_get_object(builder, "editableProfileName")));
	data->widgets->label_profile_name_error = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelProfileNameError")));

	data->widgets->stack_main = GTK_STACK(GTK_WIDGET(gtk_builder_get_object(builder, "stackMain")));
	data->widgets->box_main = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMain")));
	data->widgets->overlay_main = GTK_OVERLAY(GTK_WIDGET(gtk_builder_get_object(builder, "overlayMain")));
	data->widgets->box_connection_lost = GTK_WIDGET(gtk_builder_get_object(builder, "boxConnectionLost"));

	return builder;
}

/**
 * @brief The function used to initialize application widgets and mouse settings.
 *
 * @param app The GtkApplication instance
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

	add_mouse_profile_entries(data);

	g_signal_connect(data->widgets->window, "close-request", G_CALLBACK(close_application), data);
	g_signal_connect(data->widgets->window_new_mouse_profile, "close-request", G_CALLBACK(reset_new_profile_window), data);
	g_signal_connect(data->widgets->button_save_mouse_settings, "clicked", G_CALLBACK(save_mouse_settings), data);
	widget_add_event(builder, "buttonConfirmNewProfile", "clicked", create_new_mouse_profile, data);

	g_timeout_add(100, G_SOURCE_FUNC(update_battery_display), data);

	if (data->mouse->dev == NULL) {
		hide_mouse_settings_visibility(data);
	} else {
		load_mouse_profile_to_mouse(data);
	}

	gtk_window_set_application(data->widgets->window, app);
	gtk_window_set_transient_for(data->widgets->window_new_mouse_profile, data->widgets->window);
	gtk_window_present(data->widgets->window);
}

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

G_MODULE_EXPORT void display_new_profile_window(GtkWindow *window_new_mouse_profile, GtkButton *button) {
	gtk_window_present(window_new_mouse_profile);
}

G_MODULE_EXPORT void hide_mouse_profile_popover(GtkMenuButton *menu_button_mouse_profiles, GtkButton *button) {
	gtk_menu_button_popdown(menu_button_mouse_profiles);
}

G_MODULE_EXPORT void close_new_profile_window(GtkWindow *window_new_mouse_profile, GtkButton *button) {
	gtk_window_close(window_new_mouse_profile);
}

G_MODULE_EXPORT void open_profiles_directory() {
	GError *error = NULL;

	const char uri_prefix[] = "file://";

	char path[sizeof(uri_prefix) + PATH_MAX + sizeof(PATH_SEP PROFILE_DIR)] = {0};
	memcpy(path, uri_prefix, sizeof(uri_prefix));

	if (getcwd(path + (sizeof(uri_prefix) - 1), PATH_MAX) == NULL) return;

	strcat(path, PATH_SEP PROFILE_DIR);
	g_app_info_launch_default_for_uri(path, NULL, &error);

	if (error != NULL) debug("%s\n", error->message);
}