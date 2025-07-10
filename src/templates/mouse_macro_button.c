#include <gtk/gtk.h>
#include "mouse_macro_button.h"

G_DEFINE_TYPE(MouseMacroButton, mouse_macro_button, GTK_TYPE_BOX)

static void mouse_macro_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), MOUSE_TYPE_MACRO_BUTTON);
    G_OBJECT_CLASS(mouse_macro_button_parent_class)->dispose(gobject);
}

static void mouse_macro_button_class_init(MouseMacroButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = mouse_macro_button_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/mouse-macro-button.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    
    gtk_widget_class_bind_template_child(widget_class, MouseMacroButton, button_name);
    gtk_widget_class_bind_template_child(widget_class, MouseMacroButton, button_edit);
    gtk_widget_class_bind_template_child(widget_class, MouseMacroButton, button_delete);
}

static void mouse_macro_button_init(MouseMacroButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MouseMacroButton* mouse_macro_button_new(char *name, int index) {
    MouseMacroButton *self = g_object_new(MOUSE_TYPE_MACRO_BUTTON, "name", name, NULL);

    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_name), (const char*) G_VARIANT_TYPE_UINT32, index);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_delete), (const char*) G_VARIANT_TYPE_UINT32, index);

    return self;
}