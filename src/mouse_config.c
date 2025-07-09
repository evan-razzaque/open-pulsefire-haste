#include <gtk/gtk.h>
#include "types.h"

#include "mouse_config.h"

void menu_button_set_popover_visibility(GtkMenuButton *self, bool visible) {
    if (visible) {
        gtk_menu_button_popup(self);
    } else {
        gtk_menu_button_popdown(self);
    }
}