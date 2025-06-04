#ifndef ENUMS_H
#define ENUMS_H

enum MOUSE_REPORT {
    REPORT_CONNECTION           = 0x46,
    REPORT_HARDWARE             = 0x50,
    REPORT_HEARTBEAT            = 0x51,
    REPORT_ONBOARD_LED_SETTINGS = 0x52
} typedef MOUSE_REPORT;

enum MOUSE_BUTTON {
	LEFT_MOUSE_BUTTON,
	RIGHT_MOUSE_BUTTON,
	MIDDLE_MOUSE_BUTTON,
	SIDE_BUTTON_BACK,
	SIDE_BUTTON_FORWARD,
	DPI_BUTTON
} typedef MOUSE_BUTTON;

enum MOUSE_ACTION {
    DISABLED     = 0x0000,

    LEFT_CLICK   = 0x0101,
    RIGHT_CLICK  = 0x0102,
    MIDDLE_CLICK = 0x0103,
    BACK         = 0x0104,
    FORWARD      = 0x0105,

    KEY_A        = 0x0204, // TODO: add special keys

    PLAY_PAUSE   = 0x0300,
    STOP         = 0x0301,
    PREVIOUS     = 0x0302,
    NEXT         = 0x0303,
    MUTE         = 0x0304,
    VOLUME_DOWN  = 0x0305,
    VOLUME_UP    = 0x0306,
    
    DPI_SWITCH   = 0x0708
} typedef MOUSE_ACTION;

enum SEND_BYTE {
    SEND_POLLING_RATE        = 0xd0,
    SEND_LOWER_POWER_WARNING = 0xd1, // TODO: verify value
    SEND_LED                 = 0xd2,
    SEND_DPI                 = 0xd3,
    SEND_BUTTON_ASSIGNMENT   = 0xd4,
    SEND_MACRO_ASSIGNMENT    = 0xd5,
    SEND_MACRO_DATA          = 0xd6,
    SAVE_SETTINGS            = 0xda
} typedef SEND_BYTE;

#endif