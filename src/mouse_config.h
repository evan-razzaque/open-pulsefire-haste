#ifndef MOUSE_CONFIG_H
#define MOUSE_CONFIG_H

#include <gtk/gtk.h>
#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data)\
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

/**
 * Convieniece macro for a key(s) down event
 *
 * @param modifier The modifier keys
 * @param delay long to wait before executing the next event (in milliseconds)
 * @param key_ids HID key usage ids (max 6)
 * @return A macro event object
 */
#define KEYBOARD_EVENT_DOWN(modifier, delay, key_ids...)\
    (macro_event) {.key_event = {.event_type = MACRO_EVENT_TYPE_KEYBOARD, .modifier_keys = modifier, .keys = {key_ids}, .delay_next_action = delay}}

/**
 * Convieniece macro for a key(s) up event
 *
 * @param delay long to wait before executing the next event (in milliseconds)
 * @return A macro event object
 */
#define KEYBOARD_EVENT_UP(delay)\
    (macro_event) {.key_event = {.event_type = MACRO_EVENT_TYPE_KEYBOARD, .delay_next_action = delay}}

/**
 * Convieniece macro for a mouse event.
 *   
 * @param btn The mouse button
 * @param duration The duration of mouse down event
 * @param delay How long to wait before executing the next event (in milliseconds)
 */
#define MOUSE_EVENT(btn, duration, delay)\
    (macro_event) {.mouse_event = {\
        .down = {.event_type = MACRO_EVENT_TYPE_MOUSE, .button = btn, .delay_next_action = duration},\
        .up = {.event_type = MACRO_EVENT_TYPE_MOUSE, .delay_next_action = delay}\
    }}

/**
 * Init for mouse led settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_led_init(GtkBuilder *builder, app_data *data);

/**
 * Init for mouse button remapping.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_buttons_init(GtkBuilder *builder, app_data *data);

/**
 * Init for macros.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_macro_init(GtkBuilder *builder, app_data *data);

#endif
