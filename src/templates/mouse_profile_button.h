#ifndef MOUSE_PROFILE_BUTTON_H
#define MOUSE_PROFILE_BUTTON_H

#include <gtk/gtk.h>
#include <application.h>

G_DECLARE_FINAL_TYPE(MouseProfileButton, mouse_profile_button, MOUSE, PROFILE_BUTTON, GtkBox)

#define MOUSE_TYPE_PROFILE_BUTTON (mouse_profile_button_get_type())
#define MOUSE_PROFILE_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MOUSE_TYPE_PROFILE_BUTTON, MouseProfileButton))

/**
 * @brief Creates a MouseProfileButton.
 * 
 * @param name The name of the mouse profile
 * @param is_default_profile Whether the mouse profile is the default profile or not
 * @return a new MouseProfileButton
 */
MouseProfileButton* mouse_profile_button_new(char *name, bool is_default_profile);

#endif