#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <math.h>

#include "mouse.h"
#include "buttons.h"

#define min(a, b) (((a) < (b))? (a) : (b))

int assign_button_action(hid_device *dev, MOUSE_BUTTON button, uint16_t action) {
	byte type = action >> 8;
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_BUTTON_ASSIGNMENT), button, type, 0x02, action};
	
	return mouse_write(dev, data);
}

static int set_macro_assignment(hid_device *dev, MOUSE_BUTTON button, MACRO_REPEAT_MODE repeat_mode, int event_count) {
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_MACRO_ASSIGNMENT), button, 0x00, 0x05, event_count, 0x00, repeat_mode};
	int res;

	print_data(data);
	res = mouse_write(dev, data);

	return res;
}

int assign_button_macro(hid_device *dev, MACRO_BINDING binding, MACRO_REPEAT_MODE repeat_mode, macro_event *events, int event_count) {
	byte button = binding;
	int res;

	res = assign_button_action(dev, button, binding);

	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_MACRO_DATA), button, 0x00, event_count};

	int events_remaining = event_count;
	int packet_count = ceil(event_count / 6.0);
	int i = 0;

	// Macro data packets
	for (int p = 0; p < packet_count; p++) {
		data[FIRST_BYTE + 2] = MACRO_PACKET_SUM(p);
		data[FIRST_BYTE + 3] = min(events_remaining, 6) + MACRO_PACKET_EVENT_COUNT(p);
		
		int offset = FIRST_BYTE + 4;
		
		// Macro events
		for (int j = 6; j > 0 && events_remaining > 0;) {
			/**
			 * Since the first byte of a generic macro event is the macro type,
			 * and MACRO_EVENT_TYPE_KEYBOARD = 0x1a and MACRO_EVENT_TYPE_MOUSE = 0x25,
			 * we can shift the byte right 4 bits to obtain the actual number of events.
			 */
			int actual_event_count = events[i].event_data[0] >> 4;
			
			events_remaining -= actual_event_count;
			j -= actual_event_count;
			
			memcpy(data + offset, events[i].event_data, MACRO_EVENT_SIZE);
			offset += MACRO_EVENT_SIZE;
			i++;
		}

		memset(data + offset, 0, PACKET_SIZE - offset); // Zeros out unessesary event bytes from previous packets
		print_data(data);
		res = mouse_write(dev, data);
	}

	res = set_macro_assignment(dev, button, repeat_mode, event_count);

	return res;
}