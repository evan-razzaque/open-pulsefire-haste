#include <stdint.h>
#include "enums.h"

#ifndef OPTIONS_H
#define OPTIONS_H

struct color_options {
	uint8_t red; 
	uint8_t green; 
	uint8_t blue;
	uint8_t brightness;
} typedef color_options;

struct button_options {
	MOUSE_BUTTON button;
	ASSIGNMENT_TYPE type;
	uint8_t action;
} typedef button_options;

#endif