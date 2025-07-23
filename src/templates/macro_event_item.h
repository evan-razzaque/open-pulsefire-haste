#ifndef MACRO_EVENT_ITEM_H
#define MACRO_EVENT_ITEM_H

#include <gtk/gtk.h>
#include <stdint.h>

#include "macro_types.h"

G_DECLARE_FINAL_TYPE(MacroEventItem, macro_event_item, MACRO, EVENT_ITEM, GtkBox)

#define MACRO_TYPE_EVENT_ITEM (macro_event_item_get_type())
#define MACRO_EVENT_ITEM(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MACRO_TYPE_EVENT_ITEM, MacroEventItem))

MacroEventItem* macro_event_item_new(const char* action_name, uint16_t delay, MACRO_EVENT_TYPE event_type);

#endif
