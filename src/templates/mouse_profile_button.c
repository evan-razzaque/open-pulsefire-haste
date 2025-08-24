#include <gtk/gtk.h>
#include "mouse_profile_button.h"

struct _MouseProfileButton {
    GtkBox parent_instance;
    
    GtkButton *button_name; 
    GtkButton *button_edit;
    GtkButton *button_delete;
};

G_DEFINE_TYPE(MouseProfileButton, mouse_profile_button, GTK_TYPE_BOX)

static void mouse_profile_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), MOUSE_TYPE_PROFILE_BUTTON);
    G_OBJECT_CLASS(mouse_profile_button_parent_class)->dispose(gobject);
}

static void mouse_profile_button_class_init(MouseProfileButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = mouse_profile_button_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/mouse-profile-button.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_name);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_edit);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_delete);
}

static void mouse_profile_button_init(MouseProfileButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MouseProfileButton* mouse_profile_button_new(char *name, bool is_default_profile) {
    MouseProfileButton *self = g_object_new(MOUSE_TYPE_PROFILE_BUTTON, "name", name, NULL);
    
    gtk_widget_set_visible(GTK_WIDGET(self->button_edit), !is_default_profile);
    gtk_widget_set_visible(GTK_WIDGET(self->button_delete), !is_default_profile);
    
    return self;
}