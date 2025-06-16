#ifndef HID_KEYBOARD_MAP_H
#define HID_KEYBOARD_MAP_H

#include <stdint.h>
#include <gtk-4.0/gdk/gdkkeysyms.h>

/**
 * @brief Used to initialize an array for mapping gdk key values to hid usage ids.
 * 
 */
#define KEYBOARD_MAP() {\
    [GDK_KEY_a]            = 0x04,\
    [GDK_KEY_b]            = 0x05,\
    [GDK_KEY_c]            = 0x06,\
    [GDK_KEY_d]            = 0x07,\
    [GDK_KEY_e]            = 0x08,\
    [GDK_KEY_f]            = 0x09,\
    [GDK_KEY_g]            = 0x0A,\
    [GDK_KEY_h]            = 0x0B,\
    [GDK_KEY_i]            = 0x0C,\
    [GDK_KEY_j]            = 0x0D,\
    [GDK_KEY_k]            = 0x0E,\
    [GDK_KEY_l]            = 0x0F,\
    [GDK_KEY_m]            = 0x10,\
    [GDK_KEY_n]            = 0x11,\
    [GDK_KEY_o]            = 0x12,\
    [GDK_KEY_p]            = 0x13,\
    [GDK_KEY_q]            = 0x14,\
    [GDK_KEY_r]            = 0x15,\
    [GDK_KEY_s]            = 0x16,\
    [GDK_KEY_t]            = 0x17,\
    [GDK_KEY_u]            = 0x18,\
    [GDK_KEY_v]            = 0x19,\
    [GDK_KEY_w]            = 0x1A,\
    [GDK_KEY_x]            = 0x1B,\
    [GDK_KEY_y]            = 0x1C,\
    [GDK_KEY_z]            = 0x1D,\
    \
    [GDK_KEY_1]            = 0x1E,\
    [GDK_KEY_2]            = 0x1F,\
    [GDK_KEY_3]            = 0x20,\
    [GDK_KEY_4]            = 0x21,\
    [GDK_KEY_5]            = 0x22,\
    [GDK_KEY_6]            = 0x23,\
    [GDK_KEY_7]            = 0x24,\
    [GDK_KEY_8]            = 0x25,\
    [GDK_KEY_9]            = 0x26,\
    [GDK_KEY_0]            = 0x27,\
    \
    [GDK_KEY_exclam]       = 0x1E,\
    [GDK_KEY_at]           = 0x1F,\
    [GDK_KEY_numbersign]   = 0x20,\
    [GDK_KEY_dollar]       = 0x21,\
    [GDK_KEY_percent]      = 0x22,\
    [GDK_KEY_caret]        = 0x23,\
    [GDK_KEY_ampersand]    = 0x24,\
    [GDK_KEY_asterisk]     = 0x25,\
    [GDK_KEY_parenleft]    = 0x26,\
    [GDK_KEY_parenright]   = 0x27,\
    \
    [GDK_KEY_Return]       = 0x28,\
    [GDK_KEY_Escape]       = 0x29,\
    [GDK_KEY_BackSpace]    = 0x2A,\
    [GDK_KEY_Tab]          = 0x2B,\
    [GDK_KEY_space]        = 0x2C,\
    \
    [GDK_KEY_minus]        = 0x2D,\
    [GDK_KEY_equal]        = 0x2E,\
    [GDK_KEY_bracketleft]  = 0x2F,\
    [GDK_KEY_bracketright] = 0x30,\
    [GDK_KEY_backslash]    = 0x31,\
    [GDK_KEY_semicolon]    = 0x33,\
    [GDK_KEY_quoteright]   = 0x34,\
    [GDK_KEY_grave]        = 0x35,\
    [GDK_KEY_comma]        = 0x36,\
    [GDK_KEY_period]       = 0x37,\
    [GDK_KEY_slash]        = 0x38,\
    \
    [GDK_KEY_underscore]   = 0x2D,\
    [GDK_KEY_plus]         = 0x2E,\
    [GDK_KEY_braceleft]    = 0x2F,\
    [GDK_KEY_braceright]   = 0x30,\
    [GDK_KEY_bar]          = 0x31,\
    [GDK_KEY_colon]        = 0x33,\
    [GDK_KEY_quotedbl]     = 0x34,\
    [GDK_KEY_asciitilde]   = 0x35,\
    [GDK_KEY_less]         = 0x36,\
    [GDK_KEY_greater]      = 0x37,\
    [GDK_KEY_question]     = 0x38,\
    \
    [GDK_KEY_Caps_Lock]    = 0x39,\
    [GDK_KEY_F1]           = 0x3A,\
    [GDK_KEY_F2]           = 0x3B,\
    [GDK_KEY_F3]           = 0x3C,\
    [GDK_KEY_F4]           = 0x3D,\
    [GDK_KEY_F5]           = 0x3E,\
    [GDK_KEY_F6]           = 0x3F,\
    [GDK_KEY_F7]           = 0x40,\
    [GDK_KEY_F8]           = 0x41,\
    [GDK_KEY_F9]           = 0x42,\
    [GDK_KEY_F10]          = 0x43,\
    [GDK_KEY_F11]          = 0x44,\
    [GDK_KEY_F12]          = 0x45,\
    [GDK_KEY_Print]        = 0x46,\
    [GDK_KEY_Scroll_Lock]  = 0x47,\
    [GDK_KEY_Pause]        = 0x48,\
    [GDK_KEY_Insert]       = 0x49,\
    [GDK_KEY_Home]         = 0x4A,\
    [GDK_KEY_Page_Up]      = 0x4B,\
    [GDK_KEY_Delete]       = 0x4C,\
    [GDK_KEY_End]          = 0x4D,\
    [GDK_KEY_Page_Down]    = 0x4E,\
    [GDK_KEY_Right]        = 0x4F,\
    [GDK_KEY_Left]         = 0x50,\
    [GDK_KEY_Down]         = 0x51,\
    [GDK_KEY_Up]           = 0x52,\
    [GDK_KEY_Num_Lock]     = 0x53,\
    [GDK_KEY_KP_Divide]    = 0x54,\
    [GDK_KEY_KP_Multiply]  = 0x55,\
    [GDK_KEY_KP_Subtract]  = 0x56,\
    [GDK_KEY_KP_Add]       = 0x57,\
    [GDK_KEY_KP_Enter]     = 0x58,\
    \
    [GDK_KEY_KP_1]         = 0x59,\
    [GDK_KEY_KP_End]       = 0x59,\
    \
    [GDK_KEY_KP_2]         = 0x5A,\
    [GDK_KEY_KP_Down]      = 0x5A,\
    \
    [GDK_KEY_KP_3]         = 0x5B,\
    [GDK_KEY_KP_Page_Down] = 0x5B,\
    \
    [GDK_KEY_KP_4]         = 0x5C,\
    [GDK_KEY_KP_Left]      = 0x5C,\
    \
    [GDK_KEY_KP_5]         = 0x5D,\
    \
    [GDK_KEY_KP_6]         = 0x5E,\
    [GDK_KEY_KP_Right]     = 0x5E,\
    \
    [GDK_KEY_KP_7]         = 0x5F,\
    [GDK_KEY_KP_Home]      = 0x5F,\
    \
    [GDK_KEY_KP_8]         = 0x60,\
    [GDK_KEY_KP_Up]        = 0x60,\
    \
    [GDK_KEY_KP_9]         = 0x61,\
    [GDK_KEY_KP_Page_Up]   = 0x61,\
    \
    [GDK_KEY_KP_0]         = 0x62,\
    [GDK_KEY_KP_Insert]    = 0x62,\
    \
    [GDK_KEY_KP_Decimal]   = 0x63,\
    [GDK_KEY_KP_Delete]    = 0x63,\
    \
    [GDK_KEY_Menu]         = 0x64,\
    \
    [GDK_KEY_Control_L]    = 0xE0,\
    [GDK_KEY_Shift_L]      = 0xE1,\
    [GDK_KEY_Alt_L]        = 0xE2,\
    [GDK_KEY_Control_R]    = 0xE4,\
    [GDK_KEY_Shift_R]      = 0xE5,\
    [GDK_KEY_Alt_R]        = 0xE6\
}

#define KEY_NAMES() {\
    [0x04] = "A",\
    [0x05] = "B",\
    [0x06] = "C",\
    [0x07] = "D",\
    [0x08] = "E",\
    [0x09] = "F",\
    [0x0A] = "G",\
    [0x0B] = "H",\
    [0x0C] = "I",\
    [0x0D] = "J",\
    [0x0E] = "K",\
    [0x0F] = "L",\
    [0x10] = "M",\
    [0x11] = "N",\
    [0x12] = "O",\
    [0x13] = "P",\
    [0x14] = "Q",\
    [0x15] = "R",\
    [0x16] = "S",\
    [0x17] = "T",\
    [0x18] = "U",\
    [0x19] = "V",\
    [0x1A] = "W",\
    [0x1B] = "X",\
    [0x1C] = "Y",\
    [0x1D] = "Z",\
    [0x1E] = "1",\
    [0x1F] = "2",\
    [0x20] = "3",\
    [0x21] = "4",\
    [0x22] = "5",\
    [0x23] = "6",\
    [0x24] = "7",\
    [0x25] = "8",\
    [0x26] = "9",\
    [0x27] = "0",\
    [0x28] = "Enter",\
    [0x29] = "Escape",\
    [0x2A] = "Backspace",\
    [0x2B] = "Tab",\
    [0x2C] = "Space",\
    [0x2D] = "-",\
    [0x2E] = "=",\
    [0x2F] = "[",\
    [0x30] = "]",\
    [0x31] = "\\",\
    [0x33] = ";",\
    [0x34] = "'",\
    [0x35] = "`",\
    [0x36] = ",",\
    [0x37] = ".",\
    [0x38] = "/",\
    [0x39] = "Caps Lock",\
    [0x3A] = "F1",\
    [0x3B] = "F2",\
    [0x3C] = "F3",\
    [0x3D] = "F4",\
    [0x3E] = "F5",\
    [0x3F] = "F6",\
    [0x40] = "F7",\
    [0x41] = "F8",\
    [0x42] = "F9",\
    [0x43] = "F10",\
    [0x44] = "F11",\
    [0x45] = "F12",\
    [0x46] = "Print",\
    [0x47] = "Scroll Lock",\
    [0x48] = "Pause",\
    [0x49] = "Insert",\
    [0x4A] = "Home",\
    [0x4B] = "Page Up",\
    [0x4C] = "Delete",\
    [0x4D] = "End",\
    [0x4E] = "Page Down",\
    [0x4F] = "Right",\
    [0x50] = "Left",\
    [0x51] = "Down",\
    [0x52] = "Up",\
    [0x53] = "Num Lock",\
    [0x54] = "Keypad /",\
    [0x55] = "Keypad *",\
    [0x56] = "Keypad -",\
    [0x57] = "Keypad +",\
    [0x58] = "Keypad Enter",\
    [0x59] = "Keypad 1",\
    [0x5A] = "Keypad 2",\
    [0x5B] = "Keypad 3",\
    [0x5C] = "Keypad 4",\
    [0x5D] = "Keypad 5",\
    [0x5E] = "Keypad 6",\
    [0x5F] = "Keypad 7",\
    [0x60] = "Keypad 8",\
    [0x61] = "Keypad 9",\
    [0x62] = "Keypad 0",\
    [0x63] = "Keypad Decimal",\
    [0x64] = "Menu",\
    [0xE0] = "L-CTRL",\
    [0xE1] = "L-SHIFT",\
    [0xE2] = "L-ALT",\
    [0xE4] = "R-CTRL",\
    [0xE5] = "R-SHIFT",\
    [0xE6] = "R-ALT"\
}

#endif