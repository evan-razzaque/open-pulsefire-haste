#include <time.h>
#include "util.h"

time_t clock_gettime_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    
    return (t.tv_sec * 1000) + (t.tv_nsec / (1000 * 1000));
}

void gtk_spin_button_hide_buttons(GtkSpinButton *self) {
    GtkWidget *button_decrease_value = gtk_widget_get_next_sibling(
        gtk_widget_get_first_child(GTK_WIDGET(self))
    );
    GtkWidget *button_increase_value = gtk_widget_get_next_sibling(button_decrease_value);
    
    gtk_widget_set_visible(button_decrease_value, false);
    gtk_widget_set_visible(button_increase_value, false);
}

