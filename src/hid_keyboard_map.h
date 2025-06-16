#include <stdint.h>
#include <gtk-4.0/gdk/gdkkeysyms.h>

/**
 * @brief Used to initialize an array for mapping gdk key values to hid usage ids.
 * 
 */
#define KEYBOARD_MAP() {\
    [(uint16_t) GDK_KEY_a]            = 0x04,\
    [(uint16_t) GDK_KEY_b]            = 0x05,\
    [(uint16_t) GDK_KEY_c]            = 0x06,\
    [(uint16_t) GDK_KEY_d]            = 0x07,\
    [(uint16_t) GDK_KEY_e]            = 0x08,\
    [(uint16_t) GDK_KEY_f]            = 0x09,\
    [(uint16_t) GDK_KEY_g]            = 0x0A,\
    [(uint16_t) GDK_KEY_h]            = 0x0B,\
    [(uint16_t) GDK_KEY_i]            = 0x0C,\
    [(uint16_t) GDK_KEY_j]            = 0x0D,\
    [(uint16_t) GDK_KEY_k]            = 0x0E,\
    [(uint16_t) GDK_KEY_l]            = 0x0F,\
    [(uint16_t) GDK_KEY_m]            = 0x10,\
    [(uint16_t) GDK_KEY_n]            = 0x11,\
    [(uint16_t) GDK_KEY_o]            = 0x12,\
    [(uint16_t) GDK_KEY_p]            = 0x13,\
    [(uint16_t) GDK_KEY_q]            = 0x14,\
    [(uint16_t) GDK_KEY_r]            = 0x15,\
    [(uint16_t) GDK_KEY_s]            = 0x16,\
    [(uint16_t) GDK_KEY_t]            = 0x17,\
    [(uint16_t) GDK_KEY_u]            = 0x18,\
    [(uint16_t) GDK_KEY_v]            = 0x19,\
    [(uint16_t) GDK_KEY_w]            = 0x1A,\
    [(uint16_t) GDK_KEY_x]            = 0x1B,\
    [(uint16_t) GDK_KEY_y]            = 0x1C,\
    [(uint16_t) GDK_KEY_z]            = 0x1D,\
    \
    [(uint16_t) GDK_KEY_1]            = 0x1E,\
    [(uint16_t) GDK_KEY_2]            = 0x1F,\
    [(uint16_t) GDK_KEY_3]            = 0x20,\
    [(uint16_t) GDK_KEY_4]            = 0x21,\
    [(uint16_t) GDK_KEY_5]            = 0x22,\
    [(uint16_t) GDK_KEY_6]            = 0x23,\
    [(uint16_t) GDK_KEY_7]            = 0x24,\
    [(uint16_t) GDK_KEY_8]            = 0x25,\
    [(uint16_t) GDK_KEY_9]            = 0x26,\
    [(uint16_t) GDK_KEY_0]            = 0x27,\
    \
    [(uint16_t) GDK_KEY_exclam]       = 0x1E,\
    [(uint16_t) GDK_KEY_at]           = 0x1F,\
    [(uint16_t) GDK_KEY_numbersign]   = 0x20,\
    [(uint16_t) GDK_KEY_dollar]       = 0x21,\
    [(uint16_t) GDK_KEY_percent]      = 0x22,\
    [(uint16_t) GDK_KEY_caret]        = 0x23,\
    [(uint16_t) GDK_KEY_ampersand]    = 0x24,\
    [(uint16_t) GDK_KEY_asterisk]     = 0x25,\
    [(uint16_t) GDK_KEY_parenleft]    = 0x26,\
    [(uint16_t) GDK_KEY_parenright]   = 0x27,\
    \
    [(uint16_t) GDK_KEY_Return]       = 0x28,\
    [(uint16_t) GDK_KEY_Escape]       = 0x29,\
    [(uint16_t) GDK_KEY_BackSpace]    = 0x2A,\
    [(uint16_t) GDK_KEY_Tab]          = 0x2B,\
    [(uint16_t) GDK_KEY_space]        = 0x2C,\
    \
    [(uint16_t) GDK_KEY_minus]        = 0x2D,\
    [(uint16_t) GDK_KEY_equal]        = 0x2E,\
    [(uint16_t) GDK_KEY_bracketleft]  = 0x2F,\
    [(uint16_t) GDK_KEY_bracketright] = 0x30,\
    [(uint16_t) GDK_KEY_backslash]    = 0x31,\
    [(uint16_t) GDK_KEY_semicolon]    = 0x33,\
    [(uint16_t) GDK_KEY_quoteright]   = 0x34,\
    [(uint16_t) GDK_KEY_grave]        = 0x35,\
    [(uint16_t) GDK_KEY_comma]        = 0x36,\
    [(uint16_t) GDK_KEY_period]       = 0x37,\
    [(uint16_t) GDK_KEY_slash]        = 0x38,\
    \
    [(uint16_t) GDK_KEY_underscore]   = 0x2D,\
    [(uint16_t) GDK_KEY_plus]         = 0x2E,\
    [(uint16_t) GDK_KEY_braceleft]    = 0x2F,\
    [(uint16_t) GDK_KEY_braceright]   = 0x30,\
    [(uint16_t) GDK_KEY_bar]          = 0x31,\
    [(uint16_t) GDK_KEY_colon]        = 0x33,\
    [(uint16_t) GDK_KEY_quotedbl]     = 0x34,\
    [(uint16_t) GDK_KEY_asciitilde]   = 0x35,\
    [(uint16_t) GDK_KEY_less]         = 0x36,\
    [(uint16_t) GDK_KEY_greater]      = 0x37,\
    [(uint16_t) GDK_KEY_question]     = 0x38,\
    \
    [(uint16_t) GDK_KEY_Caps_Lock]    = 0x39,\
    [(uint16_t) GDK_KEY_F1]           = 0x3A,\
    [(uint16_t) GDK_KEY_F2]           = 0x3B,\
    [(uint16_t) GDK_KEY_F3]           = 0x3C,\
    [(uint16_t) GDK_KEY_F4]           = 0x3D,\
    [(uint16_t) GDK_KEY_F5]           = 0x3E,\
    [(uint16_t) GDK_KEY_F6]           = 0x3F,\
    [(uint16_t) GDK_KEY_F7]           = 0x40,\
    [(uint16_t) GDK_KEY_F8]           = 0x41,\
    [(uint16_t) GDK_KEY_F9]           = 0x42,\
    [(uint16_t) GDK_KEY_F10]          = 0x43,\
    [(uint16_t) GDK_KEY_F11]          = 0x44,\
    [(uint16_t) GDK_KEY_F12]          = 0x45,\
    [(uint16_t) GDK_KEY_F12]          = 0x45,\
    [(uint16_t) GDK_KEY_Print]        = 0x46,\
    [(uint16_t) GDK_KEY_Scroll_Lock]  = 0x47,\
    [(uint16_t) GDK_KEY_Pause]        = 0x48,\
    [(uint16_t) GDK_KEY_Insert]       = 0x49,\
    [(uint16_t) GDK_KEY_Home]         = 0x4A,\
    [(uint16_t) GDK_KEY_Page_Up]      = 0x4B,\
    [(uint16_t) GDK_KEY_Delete]       = 0x4C,\
    [(uint16_t) GDK_KEY_End]          = 0x4D,\
    [(uint16_t) GDK_KEY_Page_Down]    = 0x4E,\
    [(uint16_t) GDK_KEY_Right]        = 0x4F,\
    [(uint16_t) GDK_KEY_Left]         = 0x50,\
    [(uint16_t) GDK_KEY_Down]         = 0x51,\
    [(uint16_t) GDK_KEY_Up]           = 0x52,\
    [(uint16_t) GDK_KEY_Num_Lock]     = 0x53,\
    [(uint16_t) GDK_KEY_KP_Divide]    = 0x54,\
    [(uint16_t) GDK_KEY_KP_Multiply]  = 0x55,\
    [(uint16_t) GDK_KEY_KP_Subtract]  = 0x56,\
    [(uint16_t) GDK_KEY_KP_Add]       = 0x57,\
    [(uint16_t) GDK_KEY_KP_Enter]     = 0x58,\
    \
    [(uint16_t) GDK_KEY_KP_1]         = 0x59,\
    [(uint16_t) GDK_KEY_KP_End]       = 0x59,\
    \
    [(uint16_t) GDK_KEY_KP_2]         = 0x5A,\
    [(uint16_t) GDK_KEY_KP_Down]      = 0x5A,\
    \
    [(uint16_t) GDK_KEY_KP_3]         = 0x5B,\
    [(uint16_t) GDK_KEY_KP_Page_Down] = 0x5B,\
    \
    [(uint16_t) GDK_KEY_KP_4]         = 0x5C,\
    [(uint16_t) GDK_KEY_KP_Left]      = 0x5C,\
    \
    [(uint16_t) GDK_KEY_KP_5]         = 0x5D,\
    \
    [(uint16_t) GDK_KEY_KP_6]         = 0x5E,\
    [(uint16_t) GDK_KEY_KP_Right]     = 0x5E,\
    \
    [(uint16_t) GDK_KEY_KP_7]         = 0x5F,\
    [(uint16_t) GDK_KEY_KP_Home]      = 0x5F,\
    \
    [(uint16_t) GDK_KEY_KP_8]         = 0x60,\
    [(uint16_t) GDK_KEY_KP_Up]        = 0x60,\
    \
    [(uint16_t) GDK_KEY_KP_9]         = 0x61,\
    [(uint16_t) GDK_KEY_KP_Page_Up]   = 0x61,\
    \
    [(uint16_t) GDK_KEY_KP_0]         = 0x62,\
    [(uint16_t) GDK_KEY_KP_Insert]    = 0x62,\
    \
    [(uint16_t) GDK_KEY_KP_Decimal]   = 0x63,\
    [(uint16_t) GDK_KEY_KP_Delete]    = 0x63,\
    \
    [(uint16_t) GDK_KEY_Menu]         = 0x64,\
    \
    [(uint16_t) GDK_KEY_Control_L]    = 0xE0,\
    [(uint16_t) GDK_KEY_Shift_L]      = 0xE1,\
    [(uint16_t) GDK_KEY_Alt_L]        = 0xE2,\
    [(uint16_t) GDK_KEY_Control_R]    = 0xE4,\
    [(uint16_t) GDK_KEY_Shift_R]      = 0xE5,\
    [(uint16_t) GDK_KEY_Alt_R]        = 0xE6\
}