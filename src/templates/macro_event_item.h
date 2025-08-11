#ifndef MACRO_EVENT_ITEM_H
#define MACRO_EVENT_ITEM_H

#include <gtk/gtk.h>
#include <stdint.h>

#include "config_macro.h"

G_DECLARE_FINAL_TYPE(MacroEventItem, macro_event_item, MACRO, EVENT_ITEM, GtkBox)

#define MACRO_TYPE_EVENT_ITEM (macro_event_item_get_type())
#define MACRO_EVENT_ITEM(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MACRO_TYPE_EVENT_ITEM, MacroEventItem))

/**
 * @brief Emitted when the user changes the delay of a macro event item.
 * 
 * @param self The MacroEventItem instance
 * @param event_index The index of the macro event
 * @param delay The updated delay
 */
void delay_changed(
    MacroEventItem *self,
    int event_index,
    int delay,
    gpointer user_data
);

/**
 * @brief Creates a MacroEventItem.
 * 
 * @param action_name The name of the action.
 * @param delay The delay of this action
 * @param event_type A MACRO_EVENT_TYPE value
 * @param event_index The index of the macro event
 * @return a new MacroEventItem
 */
MacroEventItem* macro_event_item_new(const char* action_name, uint16_t delay, MACRO_EVENT_TYPE event_type, int event_index);

#endif
