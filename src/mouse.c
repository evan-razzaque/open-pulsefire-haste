#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>

#include "mouse.h"

void print_data(byte *data) {
	for (int i = 0; i < PACKET_SIZE; i++) {
		printf("%.2x ", data[i]);
	}

	printf("\n");
}

struct hid_device_info* get_devices(CONNECTION_TYPE *connection_type) {
	int type = CONNECTION_TYPE_WIRED;

	struct hid_device_info *dev_list = hid_enumerate(VID, PID_WIRED);

	if (!dev_list) {
		dev_list = hid_enumerate(VID, PID_WIRELESS);
		type = CONNECTION_TYPE_WIRELESS;
	}

	if (connection_type != NULL) *connection_type = type;
	return dev_list;
}

hid_device* open_device(CONNECTION_TYPE *connection_type) {
	struct hid_device_info *dev_list, *dev_info;
	hid_device *dev = NULL;

	dev_list = get_devices(connection_type);
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

int mouse_write(hid_device *dev, byte *data) {
	int bytes_written = hid_write(dev, data, PACKET_SIZE);
	
	return bytes_written;
}

int mouse_read(hid_device *dev, REPORT_BYTE reportType, byte *data) {
	int res;

	byte temp[PACKET_SIZE] = {REPORT_BYTE(reportType)};
	
	res = hid_write(dev, temp, PACKET_SIZE);

	if (res < 0) {
		printf("Error: %S\n", hid_error(dev));
		return res;
	}

	// data[PACKET_SIZE - TRUE_PACKET_SIZE] = reportType;

	do {
		res = hid_read(dev, data, PACKET_SIZE);
		if (res < 0) break;
	} while (data[FIRST_BYTE] != reportType);

	return res;
}

int save_settings(hid_device *dev, color_options *color) {
	byte d1[PACKET_SIZE] = {REPORT_BYTE(0xda), 0x01, 0x00, 0x3c, color->red, color->green, color->blue};
	mouse_write(dev, d1);

	for (int i = 0; i < 5; i++) {
		byte d2[PACKET_SIZE] = {REPORT_BYTE(0xda), 0x01, i + 1, 0x3c};
		mouse_write(dev, d2); 
	}

	byte d3[PACKET_SIZE] = {REPORT_BYTE(0xd9), 0x00, 0x00, 0x03, 0x55, 0x01, 0x23};
	mouse_write(dev, d3);

	byte d4[PACKET_SIZE] = {REPORT_BYTE(0xdb), 0x55};
	mouse_write(dev, d4);

	byte d5[PACKET_SIZE] = {REPORT_BYTE(SAVE_SETTINGS), 0xff};

	return mouse_write(dev, d5);
}