#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>

#include "mouse.h"

void print_data(uint8_t *data) {
	for (int i = 0; i < PACKET_SIZE; i++) {
		printf("%.2x ", data[i]);
	}

	printf("\n");
}

hid_device* open_device() {
	struct hid_device_info *dev_list, *dev_info;
	hid_device *dev;

	dev_list = hid_enumerate(VID, PID_WIRELESS);

	if (!dev_list) {
		dev_list = hid_enumerate(VID, PID_WIRED);
        if (!dev_list) return NULL;
	}

	dev_info = dev_list;

	while (dev_info) {
		if (dev_info->interface_number == INTERFACE) {
			dev = hid_open_path(dev_info->path);
			break;
		}

		dev_info = dev_info->next;
	}

	hid_free_enumeration(dev_list);
	return dev;
}

int mouse_write(hid_device *dev, uint8_t *data) {
	uint8_t temp[PACKET_SIZE] = {};
	int bytes_written = hid_write(dev, data, PACKET_SIZE);
	hid_read(dev, temp, PACKET_SIZE);
	
	return bytes_written;
}

int mouse_read(hid_device *dev, MOUSE_REPORT reportType, uint8_t *data) {
	int res;

	uint8_t temp[PACKET_SIZE] = {REPORT_BYTE(reportType)};
	
	res = hid_write(dev, temp, PACKET_SIZE);

	if (res < 0) {
		printf("Error: %S\n", hid_error(dev));
		return res;
	}

	data[PACKET_SIZE - TRUE_PACKET_SIZE] = reportType;
	res = hid_read(dev, data, PACKET_SIZE);

	int i = 1;

	printf("%d\n", i);

	return res;
}

int save_settings(hid_device *dev, color_options *color) {
	uint8_t d1[PACKET_SIZE] = {REPORT_BYTE(0xda), 0x01, 0x00, 0x3c, color->red, color->green, color->blue};
	mouse_write(dev, d1);

	for (int i = 0; i < 5; i++) {
		uint8_t d2[PACKET_SIZE] = {REPORT_BYTE(0xda), 0x01, i + 1, 0x3c};
		mouse_write(dev, d2); 
	}

	uint8_t d3[PACKET_SIZE] = {REPORT_BYTE(0xd9), 0x00, 0x00, 0x03, 0x55, 0x01, 0x23};
	mouse_write(dev, d3);

	uint8_t d4[PACKET_SIZE] = {REPORT_BYTE(0xdb), 0x55};
	mouse_write(dev, d4);

	uint8_t d5[PACKET_SIZE] = {REPORT_BYTE(SAVE_SETTINGS), 0xff};

	return mouse_write(dev, d5);
}