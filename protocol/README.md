# HyperX Pulsefire Haste Protocol

- [HyperX Pulsefire Haste Protocol](#hyperx-pulsefire-haste-protocol)
- [Device Info](#device-info)
	- [lsusb](#lsusb)
	- [Operation](#operation)
- [Sent Packets](#sent-packets)
	- [Polling Rate](#polling-rate)
		- [Set polling rate](#set-polling-rate)
	- [LED](#led)
		- [Set color, brightness](#set-color-brightness)
	- [DPI](#dpi)
		- [Enabled profile bitmask](#enabled-profile-bitmask)
		- [DPI profile](#dpi-profile)
			- [DPI value](#dpi-value)
			- [Profile indicator LED color](#profile-indicator-led-color)
		- [Set lift-off distance](#set-lift-off-distance)
		- [Selected DPI profile](#selected-dpi-profile)
		- [Save DPI settings](#save-dpi-settings)
	- [Buttons](#buttons)
		- [Physical button](#physical-button)
		- [Set button assignment](#set-button-assignment)
	- [Macros](#macros)
		- [Macro data](#macro-data)
		- [Macro event](#macro-event)
			- [Keyboard event](#keyboard-event)
				- [Modifier keys](#modifier-keys)
			- [Mouse event](#mouse-event)
		- [Set macro assignment](#set-macro-assignment)
	- [Saving Settings](#saving-settings)
- [Received Packets](#received-packets)
	- [Reports](#reports)
	- [Generic Event](#generic-event)
- [Button Assignment Codes](#button-assignment-codes)

# Device Info

Vendor ID: **0x03F0**<br>
Product IDs: **0x048E** (wired), **0x028E** (wireless)<br>

Interface: 2<br>
Packet Length: 64 bytes

## lsusb

TODO: fix formatting

<details>
<summary>
Wired
</summary>

```
Bus 001 Device 022: ID 03f0:048e HP, Inc HyperX Pulsefire Haste Wireless
Negotiated speed: Full Speed (12Mbps)
Device Descriptor:
 bLength								18
 bDescriptorType				 1
 bcdUSB							 2.00
 bDeviceClass						0 [unknown]
 bDeviceSubClass				 0 [unknown]
 bDeviceProtocol				 0
 bMaxPacketSize0				64
 idVendor					 0x03f0 HP, Inc
 idProduct					0x048e HyperX Pulsefire Haste Wireless
 bcdDevice					 11.11
 iManufacturer					 1 HP, Inc
 iProduct								2 HyperX Pulsefire Haste Wireless
 iSerial								 0
 bNumConfigurations			1
 Configuration Descriptor:
	bLength								 9
	bDescriptorType				 2
	wTotalLength			 0x0074
	bNumInterfaces					4
	bConfigurationValue		 1
	iConfiguration					0
	bmAttributes				 0xe0
	 Self Powered
	 Remote Wakeup
	MaxPower							500mA
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				0
	 bAlternateSetting			 0
	 bNumEndpoints					 1
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			1 Boot Interface Subclass
	 bInterfaceProtocol			2 Mouse
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			80
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x81	EP 1 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				1
	 bAlternateSetting			 0
	 bNumEndpoints					 1
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			0 [unknown]
	 bInterfaceProtocol			1 Keyboard
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			47
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x83	EP 3 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				2
	 bAlternateSetting			 0
	 bNumEndpoints					 2
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			0 [unknown]
	 bInterfaceProtocol			0
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			25
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x82	EP 2 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x02	EP 2 OUT
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				3
	 bAlternateSetting			 0
	 bNumEndpoints					 1
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			0 [unknown]
	 bInterfaceProtocol			0
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			23
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x84	EP 4 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
Device Status:		 0x0001
 Self Powered
```

</details>
<details>
<summary>Wireless</summary>

```
Bus 001 Device 021: ID 03f0:028e HP, Inc HyperX Pulsefire Haste Wireless
Negotiated speed: Full Speed (12Mbps)
Device Descriptor:
 bLength								18
 bDescriptorType				 1
 bcdUSB							 2.00
 bDeviceClass						0 [unknown]
 bDeviceSubClass				 0 [unknown]
 bDeviceProtocol				 0
 bMaxPacketSize0				64
 idVendor					 0x03f0 HP, Inc
 idProduct					0x028e HyperX Pulsefire Haste Wireless
 bcdDevice					 41.09
 iManufacturer					 1 HP, Inc
 iProduct								2 HyperX Pulsefire Haste Wireless
 iSerial								 0
 bNumConfigurations			1
 Configuration Descriptor:
	bLength								 9
	bDescriptorType				 2
	wTotalLength			 0x005b
	bNumInterfaces					3
	bConfigurationValue		 1
	iConfiguration					0
	bmAttributes				 0xa0
	 (Bus Powered)
	 Remote Wakeup
	MaxPower							500mA
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				0
	 bAlternateSetting			 0
	 bNumEndpoints					 1
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			1 Boot Interface Subclass
	 bInterfaceProtocol			2 Mouse
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			78
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x81	EP 1 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x000b	1x 11 bytes
		bInterval							 1
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				1
	 bAlternateSetting			 0
	 bNumEndpoints					 1
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			0 [unknown]
	 bInterfaceProtocol			1 Keyboard
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			92
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x82	EP 2 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0010	1x 16 bytes
		bInterval							 1
	Interface Descriptor:
	 bLength								 9
	 bDescriptorType				 4
	 bInterfaceNumber				2
	 bAlternateSetting			 0
	 bNumEndpoints					 2
	 bInterfaceClass				 3 Human Interface Device
	 bInterfaceSubClass			0 [unknown]
	 bInterfaceProtocol			0
	 iInterface							0
		HID Device Descriptor:
		 bLength								 9
		 bDescriptorType				33
		 bcdHID							 1.11
		 bCountryCode						0 Not supported
		 bNumDescriptors				 1
		 bDescriptorType				34 (null)
		 wDescriptorLength			34
		 Report Descriptors:
			** UNAVAILABLE **
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x83	EP 3 IN
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
	 Endpoint Descriptor:
		bLength								 7
		bDescriptorType				 5
		bEndpointAddress		 0x04	EP 4 OUT
		bmAttributes						3
		 Transfer Type						Interrupt
		 Synch Type							 None
		 Usage Type							 Data
		wMaxPacketSize		 0x0040	1x 64 bytes
		bInterval							 1
Device Status:		 0x0000
 (Bus Powered)
```

</details>

## Operation

This mouse operates in "direct" mode, and seems to use LED updates to persist its settings while awake. That is, a couple of seconds after sending a LED packet, the mouse reverts back to the settings saved in its onboard memory. Therefore, LED packets must be constantly sent.

# Sent Packets

Packets sent by Ngenuity.

## Polling Rate

### Set polling rate

| Byte Index | Value | Description          |
|------------|-------|----------------------|
| 0          | 0xD0  | Set polling rate     |
| 1          | 0x00  | Padding              |
| 2          | 0x00  | Padding              |
| 3          | 0x01  | 1 byte after index 3 |
| 4          | 0x0*  | Polling Rate Value  <ul><li>0x00 = 125Hz</li><li>0x01 = 250Hz</li><li>0x02 = 500Hz</li><li>0x03 = 1000Hz</li></ul> |

## LED

Since Ngenuity constantly sends LED packets, every LED effect is acheived by sending individual color/brightness updates. This differs from LED settings being [saved](#saving-settings) to the mouse.

### Set color, brightness


| Byte Index | Value | Description                                             |
|------------|-------|---------------------------------------------------------|
| 0          | 0xD2  | Send LED                                                |
| 1          | 0x00  | Padding                                                 |
| 2          | 0x00  | Padding                                                 |
| 3          | 0x08  | 8 bytes after index 3                                   |
| 4          | 0xAA  | RED                                                     |
| 5          | 0xBB  | GREEN                                                   |
| 6          | 0xCC  | BLUE                                                    |
| 7          | 0xAA  | RED (Effect, unused)                                    |
| 8          | 0xBB  | GREEN (Effect, unused)                                  |
| 9          | 0xCC  | BLUE (Effect, unused)                                   |
| 10         | 0x**  | Brightness <ul><li>Min: 0x00</li><li>Max: 0x64 (100)</li><li>Step: 0x01</li></ul> |


## DPI

The DPI settings are saved as follows:

```
1. Enabled profile bitmask
2. DPI Profile(s)
3. Lift-off distance
4. Selected Profile
5. Save DPI settings
```

### Enabled profile bitmask

A 5-bit (little-endian) number, where the nth bit corresponds to profile n.

| Byte Index | Value   | Description                                         |
|------------|---------|-----------------------------------------------------|
| 0          | 0xD2    | Send DPI settings                                   |
| 1          | 0x01    | Set enabled profiles                                |
| 2          | 0x00    | Padding                                             |
| 3          | 0x01    | 1 byte after index 3                                |
| 4          | 0b11100 | Enabled profiles (in this case profile 0, 1, and 2) |

### DPI profile

Each DPI profile contains 2 packets, being its DPI value and LED color indicator.

#### DPI value

| Byte Index | Value | Description                                                     |
|------------|-------|-----------------------------------------------------------------|
| 0          | 0xD2  | Send DPI settings                                               |
| 1          | 0x01  | Set profile DPI value                                           |
| 2          | 0x0*  | Profile number                                                  |
| 3          | 0x02  | 2 bytes after index 3 (Should be 1, but Ngenuity uses 2)        |
| 4          | 0x0A  | DPI step, where the step is 100<br>10 * step -> 1000 DPI        |

#### Profile indicator LED color

| Byte Index | Value | Description                     |
|------------|-------|---------------------------------|
| 0          | 0xD2  | Send DPI settings               |
| 1          | 0x01  | Set profile LED color indicator |
| 2          | 0x00  | Profile number                  |
| 3          | 0x01  | 3 bytes after index 3           |
| 4          | 0xAA  | RED                             |
| 5          | 0xBB  | GREEN                           |
| 6          | 0xCC  | BLUE                            |

### Set lift-off distance

Distance from the surface the sensor can track on.<br>
_Why is this with DPI profiles?_

| Byte Index | Value | Description                                              |
|------------|-------|----------------------------------------------------------|
| 0          | 0xD2  | Send DPI settings                                        |
| 1          | 0x01  | Set lift-off distance                                    |
| 2          | 0x00  | Padding                                                  |
| 3          | 0x01  | 1 byte after index 3 (no clue why there's 2 bytes after) |
| 4          | 0x0*  | Lift-off distance (in millimeters)<ul><li>Low: 1mm</li><li>High: 2mm</li><ul> |
| 5          | 0x0*  | Lift-off distance                                        |

### Selected DPI profile

| Byte Index | Value | Description                 |
|------------|-------|-----------------------------|
| 0          | 0xD2  | Send DPI settings           |
| 1          | 0x00  | Set selected profile number |
| 2          | 0x00  | Padding                     |
| 3          | 0x01  | 1 byte after index 3        |
| 4          | 0x00  | Profile number              |

### Save DPI settings

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0xDE  | Save settings     |
| 1          | 0x03  | Save DPI settings |

## Buttons

### Physical button

| Button     | Value |
|------------|-------|
| Left       | 0x00  |
| Right      | 0x01  |
| Middle     | 0x02  |
| Back       | 0x03  |
| Forward    | 0x04  |
| DPI toggle | 0x05  |

### Set button assignment

| Byte Index | Value | Description            |
|------------|-------|------------------------|
| 0          | 0xD4  | Send button assignment |
| 1          | 0x00  | [Physical button](#physical-button) |
| 2          | 0x00  | Assignment type<ul><li>0x00 = Disabled<li>0x01 = Mouse functions (USB HID Button Page 0x09) </li><li>0x02 = Keyboard functions (USB HID Keyboard Page 0x07) </li><li>0x03 = Media functions (Non-standard codes) </li><li>0x04 = Macro </li><li>0x05 = Shortcut</li><li>0x07 = DPI Switch function</li></ul> |
| 3          | 0x01  | 2 bytes after index 3  |
| 4          | 0x00  | Physical button number (for macros), Assignment code or USB HID Usage ID<br>See [Button Assignment Codes](#button-assignment-codes) for more information |

## Macros

Macros are assigned as follows:
<pre>
1. <a href="#set-button-assignment">Set button assignment</a> 
2. Macro data packet(s)
3. Set macro assignment
</pre>

### Macro data

Each macro data packet contains a maximum of 6 events. The first 4 bytes are reserved, and the rest of the bytes are used for macro events.

<table>
	<tr><th>Byte index</th><th>Value</th><th>Description</th></tr>
	<tr>
		<td>0</td>
		<td>0xD6</td>
		<td>Send macro assignment</td>
	</tr>
	<tr>
		<td>1</td>
		<td>0x0*</td>
		<td><a href="#physical-button">Phyiscal button</a></td>
	</tr>
	<tr>
		<td>2</td>
		<td>0x**</td>
		<td>
			Value can be calculated with the following formula:
			$\frac{3n}{2}$, where n is the nth macro data packet (starting at 0)<br>
			<a href="#">Explanation</a>
		</td>
	</tr>
	<tr>
		<td>3</td>
		<td>0x**</td>
		<td>
			The number of events contained in this packet. If this macro packet is odd numbered, 0x80 is added to the event count.
		</td>
	</tr>
	<tr>
		<td>4 - 63</td>
		<td>6 macro events</td>
		<td>
			Each event starts with an "event type"
			<ul>
			<li>0x1A = Keyboard function
			<li>0x25 = Mouse function
		</td>
	</tr>
</table>

### Macro event
- Keyboard and mouse keycodes can be found under their respective sections in [Button Assignment Codes](#button-assignment-codes).
- Each event contains a delay that defines how long to wait before performing the executing the next event in the macro data.
- Mouse and keyboard events cannot be executed at the same time.
- Delay is stored as 16-bit number (little-endian) in milliseconds.

#### Keyboard event
A key up event is signified with an event with all 6 keycodes set to 0x00.

| Event type | Modifier keys 8-bit value | Keycode | Keycode | Keycode | Keycode | Keycode | Keycode | Delay upper byte | Delay lower byte |
| - | - | - | - | - | - | - | - | - | - |
| 0x1A | 0x** | 0x** | 0x** | 0** | 0x** | 0x** | 0x** | 0x** | 0x** |

##### Modifier keys

| Modifier key    |	Value          |
| --------------- | -------------- |
| L-CTRL          | 0b0000000**1** |
| L-SHIFT         | 0b000000**1**0 |
| L-ALT           | 0b00000**1**00 |
| L-WIN           | 0b0000**1**000 |
| R-CTRL          | 0b000**1**0000 |
| R-SHIFT         | 0b00**1**00000 |
| R-ALT           | 0b0**1**000000 |
| R-WIN           | 0b**1**0000000 |

#### Mouse event
Since mouse events cannot be performed at the same time as a key event, each mouse event consists of a mouse up and mouse down event. Thus, this is counted as 2 events.

| Event type | Keycode | Padding | Delay upper byte | Delay lower byte | Event type | Keycode | Padding | Delay upper byte | Delay lower byte |
| - | - | - | - | - | - | - | - | - | - |
| 0x25 | 0x** | 0x00 | 0x** | 0x** | 0x25 | 0x00 | 0x00 | 0x** | 0x** |

### Set macro assignment

| Byte index | Value | Description |
| ------ | ------ | ------ |
| 0x00 | 0xD5 | Set macro assignment |
| 0x01 | 0x** | [Physical button](#physical-button) |
| 0x02 | 0x00 | Unknown |
| 0x03 | 0x05 | 5 bytes after byte index 0x03 |
| 0x04 | 0x** | Number of macro events (0x01-0xFF?) |
| 0x05 | 0x00 | Unknown |
| 0x06 | 0x00 | Repeat mode <br><ul><li>0x00 = Play once </li><li>0x02 = Toggle repeat </li><li>0x03 = Hold repeat </li></ul> |
| 0x07 | 0x00 | Unknown |
| 0x08 | 0x00 | Unknown |

<a name="saving-settings">

## Saving Settings

# Received Packets

Packets sent by the mouse

## Reports

Requires a request packet to be sent with one of the follow values as the first byte:
- 0x46 = Connection status
- 0x50 = Hardware information
- 0x51 = Heartbeat
- 0x52 = Onboard LED settings
- TODO: add additional reports 

## Generic Event

Sent by the mouse without a request.

<table></table>

# Button Assignment Codes


