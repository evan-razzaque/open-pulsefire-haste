# HyperX Pulsefire Haste Protocol
Adapted from [https://github.com/santeri3700/hyperx_pulsefire_dart_reverse_engineering/blob/main/protocol/index.md](https://github.com/santeri3700/hyperx_pulsefire_dart_reverse_engineering/blob/main/protocol/index.md)

<ul>
	<li>
		<a href="#device-info">Device Info</a>
		<ul>
			<li><a href="#lsusb">lsusb</a></li>
			<li><a href="#operation">Operation</a></li>
		</ul>
	</li>
	<li>
		<a href="#sent-packets">Sent Packets</a>
		<ul>
			<li><a href="#polling-rate">Polling Rate</a></li>
			<li><details style="margin-bottom: 0" close>
				<summary><a href="#led">LED</a></summary>
				<ul>
					<li><a href="#rgb-value">RGB value</a></li>
					<li><a href="#adjusted-rgb-value">Adjusted RGB value</a></li>
					<li><a href="#set-led-settings">Set LED settings</a></li>
				</ul>
			</details></li>
			<li><details close>
				<summary><a href="#dpi">DPI</a></summary>
				<ul>
					<li><a href="#enabled-profile-bitmask">Enabled profile bitmask</a></li>
					<li>
						<a href="#dpi-profile-max-5-profiles">DPI profile</a>
						<ul>
							<li><a href="#dpi-value">DPI value</a></li>
							<li><a href="#profile-indicator-led-color">Profile indicator LED color</a></li>
						</ul>
					</li>
					<li><a href="#set-lift-off-distance">Set lift-off distance</a></li>
					<li><a href="#selected-dpi-profile">Selected DPI profile</a></li>
					<li><a href="#save-dpi-settings">Save DPI settings</a></li>
				</ul>
			</details></li>
			<li><details close>
				<summary><a href="#buttons">Buttons</a></summary>
				<ul>
					<li><a href="#physical-button">Physical button</a></li>
					<li><a href="#set-button-assignment">Set button assignment</a></li>
				</ul>
			</details></li>
			<li><details close>
				<summary><a href="#macros">Macros</a></summary>
				<ul>
					<li><a href="#macro-data">Macro data</a></li>
					<li>
						<a href="#macro-event">Macro event</a>
						<ul>
							<li><a href="#keyboard-event">Keyboard event</a></li>
							<li><a href="#mouse-event">Mouse event</a></li>
						</ul>
					</li>
					<li><a href="#set-macro-assignment">Set macro assignment</a></li>
				</ul>
			</details></li>
			<li><details close>
				<summary><a href="#saving-settings">Saving Settings</a></summary>
				<ul>
					<li><a href="#revert-led-settings">Revert LED settings</a></li>
					<li><a href="#set-fade-led-effect">Set fade LED effect</a></li>
					<li><a href="#set-led-effect">Set LED effect</a></li>
					<li><a href="#set-led-mode">Set LED mode</a></li>
					<li><a href="#unknown-packet">Unknown packet</a></li>
					<li><a href="#save-mouse-settings">Save mouse settings</a></li>
				</ul>
			</details></li>
		</ul>
	</li>
	<li>
		<a href="#received-packets">Received Packets</a>
		<ul>
			<li><details close>
				<summary><a href="#reports">Reports</a></summary>
				<ul>
					<li><a href="#connection-status">Connection status</a></li>
					<li><a href="#hardware-information">Hardware Information</a></li>
					<li><a href="#heartbeat">Heartbeat</a></li>
					<li><a href="#led-settings">LED settings</a></li>
					<li><a href="#dpi-settings">DPI settings</a></li>
					<li><a href="#button-assignments">Button assignments</a></li>
					<li><a href="#device-settings">Device settings</a></li>
				</ul>
			</details></li>
			<li><a href="#generic-event">Generic Event</a></li>
		</ul>
	</li>
	<li>
		<a href="#button-assignment-codes">Button Assignment Codes</a>
		<ul>
			<li><a href="#disabled-assignment-type--0x00">Disabled (Assignment type = 0x00)</a></li>
			<li><a href="#mouse-functions-assignment-type--0x01">Mouse functions (Assignment type = 0x01)</a></li>
			<li><a href="#keyboard-functions-assignment-type--0x02">Keyboard functions (Assignment type = 0x02)</a></li>
			<li><a href="#media-functions-assignment-type--0x03">Media functions (Assignment type = 0x03)</a></li>
			<li><a href="#shortcut-functions-assignment-type--0x05">Shortcut functions (Assignment type = 0x05)</a></li>
			<li><a href="#dpi-toggle-function-assignment-type--0x07">DPI toggle (Assignment type = 0x07)</a></li>
		</ul>
	</li>
	<li>
		<a href="#miscellaneous">Miscellaneous</a>
		<ul>
			<li><a href="#macro-data-sum-value-byte">Macro data sum value byte</a></li>
			<li><a href="#macro-event-count">Macro event count</a></li>
		</ul>
	</li>
</ul>


# Device Info

Vendor ID: **0x03F0**<br>
Product IDs: **0x048E** (wired), **0x028E** (wireless)<br>
Usage page: **0xFF00**

Interface: 2<br>
Packet Length: 64 bytes

## lsusb

<details>
<summary>Wireless</summary>

```
Bus 001 Device 007: ID 03f0:028e HP, Inc HyperX Pulsefire Haste Wireless
Negotiated speed: Full Speed (12Mbps)
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0 [unknown]
  bDeviceSubClass         0 [unknown]
  bDeviceProtocol         0
  bMaxPacketSize0        64
  idVendor           0x03f0 HP, Inc
  idProduct          0x028e HyperX Pulsefire Haste Wireless
  bcdDevice           41.09
  iManufacturer           1 HP, Inc
  iProduct                2 HyperX Pulsefire Haste Wireless
  iSerial                 0
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x005b
    bNumInterfaces          3
    bConfigurationValue     1
    iConfiguration          0
    bmAttributes         0xa0
      (Bus Powered)
      Remote Wakeup
    MaxPower              500mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      1 Boot Interface Subclass
      bInterfaceProtocol      2 Mouse
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      78
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x000b  1x 11 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 [unknown]
      bInterfaceProtocol      1 Keyboard
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      92
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x82  EP 2 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0010  1x 16 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        2
      bAlternateSetting       0
      bNumEndpoints           2
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 [unknown]
      bInterfaceProtocol      0
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      34
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x83  EP 3 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x04  EP 4 OUT
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
Device Status:     0x0000
  (Bus Powered)
```

</details>

<details>
<summary>Wired</summary>

```
Bus 001 Device 008: ID 03f0:048e HP, Inc HyperX Pulsefire Haste Wireless
Negotiated speed: Full Speed (12Mbps)
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0 [unknown]
  bDeviceSubClass         0 [unknown]
  bDeviceProtocol         0
  bMaxPacketSize0        64
  idVendor           0x03f0 HP, Inc
  idProduct          0x048e HyperX Pulsefire Haste Wireless
  bcdDevice           11.11
  iManufacturer           1 HP, Inc
  iProduct                2 HyperX Pulsefire Haste Wireless
  iSerial                 0
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x0074
    bNumInterfaces          4
    bConfigurationValue     1
    iConfiguration          0
    bmAttributes         0xe0
      Self Powered
      Remote Wakeup
    MaxPower              500mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      1 Boot Interface Subclass
      bInterfaceProtocol      2 Mouse
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      80
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 [unknown]
      bInterfaceProtocol      1 Keyboard
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      47
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x83  EP 3 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        2
      bAlternateSetting       0
      bNumEndpoints           2
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 [unknown]
      bInterfaceProtocol      0
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      25
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x82  EP 2 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x02  EP 2 OUT
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        3
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 [unknown]
      bInterfaceProtocol      0
      iInterface              0
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 (null)
          wDescriptorLength      23
          Report Descriptors:
            ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x84  EP 4 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
Device Status:     0x0001
  Self Powered
```

</details>

## Operation

This mouse operates in "direct" mode, and seems to use LED updates to persist its settings while awake. Therefore, LED packets must be constantly sent. See [Set LED settings](#set-led-settings) for more information.

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
| 4          | 0x0*  | Polling rate value  <ul><li>0x00 = 125Hz</li><li>0x01 = 250Hz</li><li>0x02 = 500Hz</li><li>0x03 = 1000Hz</li></ul> |

## LED

Since Ngenuity constantly sends LED packets, every LED effect is acheived by sending individual color updates. This differs from [Set LED effect](#set-led-effect).

### RGB value

Used in: [Set LED settings](#set-led-settings), [LED settings](#led-settings), [DPI settings](#dpi-settings)

| Byte Index | Value | Description |
|------------|-------|-------------|
| 0          | 0x**  | RED         |
| 1          | 0x**  | GREEN       |
| 2          | 0x**  | BLUE        |

### Adjusted RGB value

Used in [Set LED effect](#set-led-effect)

| Byte Index | Value | Description |
|------------|-------|-------------|
| 0          | 0x**  | `red * (brightness / 100)` |
| 1          | 0x**  | `green * (brightness / 100)` |
| 2          | 0x**  | `blue * (brightness / 100)` |

### Set LED settings

A couple of seconds after sending this packet, the mouse reverts back to the settings saved in its onboard memory.

| Byte Index | Value | Description |
|------------|-------|-------------|
| 0          | 0xD2  | Set LED     |
| 1          | 0x00  | Padding     |
| 2          | 0x00  | Padding     |
| 3          | 0x08  | 8 bytes after index 3 |
| 4-6        | [RGB](#rgb-value) | LED color |
| 7-9        | [RGB](#rgb-value) | LED effect color, unused |
| 10         | 0x**  | Brightness <ul><li>Min: 0x00</li><li>Max: 0x64 (100)</li><li>Step: 0x01</li></ul> |
| 11         | 0x00  | Unknown     |


## DPI

The DPI settings are saved as follows (in Ngenuity):

```
1. Enabled profile bitmask
2. DPI Profile(s)
3. Lift-off distance
4. Selected Profile
5. Save DPI settings
```

Note that 3 and 5 are optional. Also 1 can be sent after the DPI profiles.

### DPI step value

Used in: [DPI value](#dpi-value), [DPI settings](#dpi-settings)

To obtain the DPI step value, The DPI value (200 - 16,000) is divided by 100 (step). The dpi step is stored as a little endian value.

| Byte Index | Value | Description |
|------------|-------|-------------|
| 0          | 0x**  | DPI step value first byte |
| 1          | 0x00  | DPI step value second byte (unused for this mouse) |


### Enabled profile bitmask

A 5-bit (little-endian) number, where the nth bit corresponds to profile n.

| Byte Index | Value   | Description                                         |
|------------|---------|-----------------------------------------------------|
| 0          | 0xD3    | Send DPI settings                                   |
| 1          | 0x01    | Set enabled profiles                                |
| 2          | 0x00    | Padding                                             |
| 3          | 0x01    | 1 byte after index 3                                |
| 4          | 0b00111 | Enabled profiles (in this case profile 0, 1, and 2) |

### DPI profile (max 5 profiles)

Each DPI profile contains 2 packets, being its DPI value and LED color indicator.

#### DPI value

| Byte Index | Value | Description                                              |
|------------|-------|----------------------------------------------------------|
| 0          | 0xD3  | Send DPI settings                                        |
| 1          | 0x02  | Set profile DPI value                                    |
| 2          | 0x0*  | Profile number (0x01 - 0x04)                             |
| 3          | 0x02  | 2 bytes after index 3                                    |
| 4-5        | [DPI step value](#dpi-step-value) | DPI step value               |

#### Profile indicator LED color

| Byte Index | Value | Description                     |
|------------|-------|---------------------------------|
| 0          | 0xD3  | Send DPI settings               |
| 1          | 0x03  | Set profile LED color indicator |
| 2          | 0x0*  | Profile number (0x01 - 0x04)    |
| 3          | 0x03  | 3 bytes after index 3           |
| 4          | 0x**  | RED                             |
| 5          | 0x**  | GREEN                           |
| 6          | 0x**  | BLUE                            |

### Set lift-off distance

Distance from the surface the sensor can track on.<br>

| Byte Index | Value | Description                                              |
|------------|-------|----------------------------------------------------------|
| 0          | 0xD3  | Send DPI settings                                        |
| 1          | 0x05  | Set lift-off distance                                    |
| 2          | 0x00  | Padding                                                  |
| 3          | 0x01  | 1 byte after index 3 (no clue why there's 2 bytes after) |
| 4          | 0x0*  | Lift-off distance (in millimeters)<ul><li>Low: 1mm</li><li>High: 2mm</li><ul> |
| 5          | 0x0*  | Lift-off distance                                        |

### Selected DPI profile

| Byte Index | Value | Description                 |
|------------|-------|-----------------------------|
| 0          | 0xD3  | Send DPI settings           |
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

Used in: [Set button assignment](#set-button-assignment), [Send macro data](#macro-data), [Set macro assignment](#set-macro-assignment), [Button action](#button-action)

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
| 1          | 0x0*  | [Physical button](#physical-button) |
| 2          | 0x0*  | Assignment type<ul><li>0x00 = Disabled<li>0x01 = Mouse functions (USB HID Button Page 0x09) </li><li>0x02 = Keyboard functions (USB HID Keyboard Page 0x07) </li><li>0x03 = Media functions (Non-standard codes) </li><li>0x04 = Macro </li><li>0x05 = Shortcut functions</li><li>0x07 = DPI Toggle function</li></ul> |
| 3          | 0x02  | 2 bytes after index 3 |
| 4          | 0x**  | [Physical button](#physical-button) (for macros), Assignment code, or USB HID Usage ID<br>See [Button Assignment Codes](#button-assignment-codes) for more information |
| 5          | 0x00  | Unknown               |

## Macros

Macros are assigned as follows:
<pre>
1. <a href="#set-button-assignment">Set button assignment</a>
2. Macro data packet(s)
3. Set macro assignment
</pre>

### Macro data

Each macro data packet contains a maximum of 6 events. The first 4 bytes are reserved, and the rest of the bytes are used for macro events.

| Byte index | Value          | Description           |
|------------|----------------|-----------------------|
| 0          | 0xD6           | Send macro data       |
| 1          | 0x0*           | [Phyiscal button](#physical-button) |
| 2          | 0x**           | Some sort of order value that can be calculated with the following formula:<br>$floor(\frac{3n}{2})$, where n is the nth macro data packet (starting at 0) <br>[Details](#macro-data-sum-value-byte)|
| 3          | 0x**           | The number of events contained in this packet. If the macro data packet is odd numbered, 0x80 is added to the event count. |
| 4 - 63     | 6 [macro events](#macro-event) | Each event starts with an "event type" <ul><li>0x1A = Keyboard function</li><li>0x25 = Mouse function</li></ul> |


### Macro event
- Keyboard and mouse keycodes can be found under their respective sections in [Button Assignment Codes](#button-assignment-codes).
- Each event contains a delay that defines how long to wait before the executing the next event in the macro data.
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
| 0 | 0xD5 | Set macro assignment |
| 1 | 0x** | [Physical button](#physical-button) |
| 2 | 0x00 | Padding |
| 3 | 0x05 | 5 bytes after byte index 0x03 |
| 4 | 0x** | Number of macro events (0x01-0x6C)<br>[Details](#macro-event-count) |
| 5 | 0x00 | Unknown |
| 6 | 0x00 | Repeat mode <br><ul><li>0x00 = Play once </li><li>0x02 = Toggle repeat </li><li>0x03 = Hold repeat </li></ul> |
| 7 | 0x00 | Unknown |
| 8 | 0x00 | Unknown |


## Saving Settings

The Mouse settings are saved as follows (in Ngenuity):

```
1. Revert LED settings or Set fade LED effect
2. Set LED effect
3. Set LED mode
4. Unknown packet
5. Save settings
```

1, 4, and 5 are optional if you only want to change the LED for the mouse (unless the LED effect is fade).

### LED mode

Used in: [Set LED effect](#set-led-effect), [Set LED mode](#set-led-mode)
<br>The modes for breathing and cycle could be wrong, and needs further testing.

s = speed

| LED Mode   | Value |
|------------|-------|
| Solid      | 0x01  |
| Fade       | 0x01  |
| Breathing  | $floor\left(120-\left(1+\frac{e}{30}\right)\cdot s\right)$ <br> |
| Cycle      | $floor\left(120-0.4s\right)$  |

### Revert LED settings

Used when LED mode is not fade

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0xD2  | Set LED           |
| 1          | 0x00  | Padding           |
| 2          | 0x40  | Revert LED settings? |
| 3          | 0x08  | 8 bytes after index 3 |
| 4          | 0xFF  | White (R)         |
| 5          | 0xFF  | White (G)         |
| 6          | 0xFF  | White (B)         |
| 7          | 0x00  | Padding / RED effect (unused)  |
| 8          | 0x00  | Padding / GREEN effect (unused) |
| 9          | 0x00  | Padding / BLUE effect  (unused) |
| 10         | 0x64  | Brightness, always seems to be 100 |
| 11         | 0x00  | Unknown           |

### Set fade LED effect

Used when LED mode is fade

| Byte Index | Value | Description |
|------------|-------|-------------|
| 0          | 0xD2  | Set LED     |
| 1          | 0x00  | Padding     |
| 2          | 0x30  | Set fade LED effect |
| 3          | 0x08  | 8 bytes after index 3 |
| 4-6        | [RGB](#rgb-value) | LED Fade color |
| 7          | 0x00  | Padding     |
| 8          | 0x00  | Padding     |
| 9          | 0x00  | Padding     |
| 10         | 0x64  | Unknown/Fixed brightness? |
| 11         | 0x00  | Unknown     |

### Set LED effect

Currently, this only covers solid colors.

The LED effect is set from 6 of these packets, with each packet containing all the colors the mouse will cycle through in order to achieve the desired LED effect.

Unlike [Set LED settings](#led), the LED effect isn't reverted until the mouse is unplugged/turned off. Technically, this could be used instead of spamming Set LED setting packets, but it would be slower to change the LED effect, especially ones that are dynamic.

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0xDA  | Set LED Effect    |
| 1          | 0x**  | [LED mode](#led-mode) |
| 2          | 0x0*  | LED effect packet number (0x00-0x05) |
| 3          | 0x3c  | 60 bytes after index 3 |
| 4 - 63     | 20 [Adjusted RGB](#adjusted-rgb-value) colors | The colors that the LED will cycle through. If the LED mode is static, only 1 color should be present. If the LED mode is fade, no colors should be present. |

### Set LED mode

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0xD9  | Set LED mode?     |
| 1          | 0x00  | Padding |
| 2          | 0x00  | Padding |
| 3          | 0x03  | 3 bytes after index 3 |
| 4          | 0x55  | Byte before LED mode? |
| 5          | 0x**  | [LED mode](#led-mode) |
| 6          | 0x23  | Unknown |

### Unknown packet

This packet seems to be related to [Set LED mode](#set-led-mode). However, the values never seem to change, indicating that this *could* be left over from a different mouse where it did change (idk just guessing).

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0xDB  | Unknown           |
| 1          | 0x55  | Byte before LED mode? |

### Save mouse settings

| Byte Index | Value | Description        |
|------------|-------|--------------------|
| 0          | 0xDE  | Save               |
| 1          | 0xFF  | Save all settings  |

# Received Packets

Packets sent by the mouse

## Reports

Requires a request packet to be sent with the report's first byte.

### Connection status

| Byte Index | Value | Description        |
|------------|-------|--------------------|
| 0          | 0x46  | Connection Status  |
| 1          | 0x00  | Padding            |
| 2          | 0x00  | Padding            |
| 3          | 0x0*  | Connection type<ul><li>0x01 = Wireless connection (1 byte after byte index 0x03) </li><li>0x02 = Wired connection (2 bytes after byte index 0x03) </li></ul> |
| 4          | 0x**  | Wireless<ul><li>0x00 = Asleep</li><li>0x01 = Awake</li></ul></ul>Wired<ul><li>0xEC = Unknown</li></ul> |
| 5          | 0x**  | Wireless = 0x00, Wired = 0xAC |

### Hardware Information

| Byte Index | Value | Description           |
|------------|-------|-----------------------|
| 0          | 0x50  | Device information    |
| 1          | 0x00  | Padding               |
| 2          | 0x00  | Padding               |
| 3          | 0x3C  | 60 bytes after index 3 |
| 4          | 0x8E  | Product id lower byte |
| 5          | 0x0*  | Product id upper byte <ul><li>Wireless = 0x02<li>Wired = 0x04</ul> |
| 6          | 0xF0  | Vendor id lower byte  |
| 7          | 0x03  | Vendor id upper byte  |
| 8-11       | 0x******** | Little-endian bcd release number<ul><li>Wireless = 4.1.0.9<li>Wired = 1.1.1.1</ul> |
| 12         | 0x8E  | Unknown               |
| 13         | 0x0*  | Unknown <ul><li>Wireless = 0x03<li>Wired = 0x05</ul> |
| 14         | 0xF0  | Unknown               |
| 15         | 0x03  | Unknown               |
| 16         | 0x0*  | Number of interfaces?<ul><li>Wireless = 0x03<li>Wired = 0x04</ul> |
| 17         | 0x00  | Unknown / Padding |
| 18         | 0x0*  | Unknown <ul><li>Wireless = 0x01<li>Wired = 0x00</ul> |
| 19         | 0x0*  | Unknown <ul><li>Wireless = 0x00<li>Wired = 0x01</ul> |
| 20-52      | 0x03  | Product string (includes null-byte) |

Product string: "HyperX Pulsefire Haste Wireless"

### Heartbeat

| Byte Index | Value | Description        |
|------------|-------|--------------------|
| 0          | 0x51  | Heartbeat          |
| 1          | 0x00  | Padding            |
| 2          | 0x00  | Padding            |
| 3          | 0x09  | 9 bytes after index 3 |
| 4          | 0x**  | Battery level <ul><li>Min: 0x00</li><li>Max: 0x64 (100)</li><li>Step: 0x01</li>|
| 5          | 0x0*  | Charging Status <ul><li>Wireless = 0x00<li>Wired = 0x01<li>Wired, Fully Charged = 0x02</ul> |
| 6          | 0x0A  | Unknown            |
| 7          | 0x1D  | Unknown            |
| 8          | 0x00  | Unknown/Padding    |
| 9          | 0x**  | Unknown            |
| 10         | 0x10  | Unknown            |
| 11         | 0x02  | Unknown            |
| 12         | 0x04  | Unknown            |

### LED settings

This packet reports the led settings sent by a [Set LED settings](#set-led-settings) packet. If a led settings packet hasn't been sent in the past couple of seconds, the color will be white, and the brightness will be 100. So basically, this packet is entirely useless.

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0x52  | LED Settings      |
| 1          | 0x00  | Padding           |
| 2          | 0x00  | Padding           |
| 3          | 0x0A  | 10 bytes after index 3 |
| 4          | 0x04  | Unknown/Padding   |
| 5          | 0x00  | Unknown/Padding   |
| 6          | 0x00  | Unknown/Padding   |
| 7          | 0x**  | Brightness        |
| 8-10       | [RGB](#rgb-value) | LED color |
| 11-13      | [RGB](#rgb-value) | LED effect color, unused |

### DPI settings

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0x53  | DPI Settings      |
| 1          | 0x00  | Padding           |
| 2          | 0x00  | Padding           |
| 3          | 0x21  | 33 bytes after index 3 (off by one) |
| 4          | 0x0*  | Profile number (0x01 - 0x04) |
| 5          | 0x**  | [Enabled profile bitmask](#enabled-profile-bitmask) |
| 6          | 0x01  | Unknown           |
| 7          | 0x00  | Unknown/Padding   |
| 8          | 0xA0  | Max DPI step value? |
| 9          | 0x00  | Unknown/Padding   |
| 10         | 0x10  | Unknown           |
| 11         | 0x00  | Unknown/Padding   |
| 12-21      | 5 [DPI step values](#dpi-step-value) | The dpi step values for each profile |
| 22-36      | 5 [RGB values](#rgb-value) | The indicator colors for each profile |
| 37         | 0x0*  | Lift-off distance (in millimeters)<ul><li>Low: 1mm</li><li>High: 2mm</li><ul> |

### Button assignments

| Byte Index | Value | Description       |
|------------|-------|-------------------|
| 0          | 0x54  | Button assignments   |
| 1          | 0x00  | Padding           |
| 2          | 0x00  | Padding           |
| 3          | 0x0C  | 12 bytes after index 3? Idk why is wrong |
| 4-21       | 6 [Button actions](#button-action) | The action for each mouse button |

#### Button action

Used by: [Button assignments](#button-assignments)

| Byte index | Value | Description |
|------------|-------|-------------|
| 0          | 0x0*  | Assignment type<ul><li>0x00 = Disabled<li>0x01 = Mouse functions (USB HID Button Page 0x09) </li><li>0x02 = Keyboard functions (USB HID Keyboard Page 0x07) </li><li>0x03 = Media functions (Non-standard codes) </li><li>0x04 = Macro </li><li>0x05 = Shortcut functions</li><li>0x07 = DPI Toggle function</li></ul> |
| 1          | 0x**  | [Physical button](#physical-button) (for macros), Assignment code, or USB HID Usage ID<br>See [Button Assignment Codes](#button-assignment-codes) for more information |
| 2          | 0x**  | If the action is a mouse button or media function, the value is (1 << n), where n is the button index or the code of the media function. Otherwise, the value is equal to the previous byte. |

### Device settings

Doesn't contain every device setting, but I can't think of a better name.

| Byte Index | Value | Description        |
|------------|-------|--------------------|
| 0          | 0x50  | Device information |
| 1          | 0x03  | Get device settings? |
| 2          | 0x00  | Padding            |
| 3          | 0x31  | 49 bytes after index 3<br>(off by one again) |
| 4          | 0x04  | Unknown            |
| 5          | 0x00  | Unknown/Padding    |
| 6          | 0x04  | Unknown            |
| 7          | 0x00  | Padding            |
| 8          | 0x10  | Unknown            |
| 9          | 0x00  | Padding            |
| 10-19      | 5 [DPI step values](#dpi-step-value) | The dpi step values for each profile |
| 20-34      | 5 [RGB values](#rgb-value) | The indicator colors for each profile |
| 35-52      | 6 [Button actions](#button-action) | The action for each mouse button |
| 53         | 0x0*  | Polling rate value <ul><li>0x00 = 125Hz</li><li>0x01 = 250Hz</li><li>0x02 = 500Hz</li><li>0x03 = 1000Hz</li></ul> |

## Generic Event

Sent by the mouse without a request.

| Byte index | Value | Description |
|------------|-------|-------------|
| 0          | 0xFF  | Generic event |
| 1          | 0x03  | Unknown |
| 2          | 0x0*  | Selected dpi profile number (0x00-0x04) |
| 3          | 0x0*  | Charging Status <ul><li>Wireless = 0x00<li>Wired = 0x01<li>Wired, Fully Charged = 0x02</ul> |
| 4          | 0x0*  | Only applies to wireless connection<ul><li>0x00 = Going to sleep </li><li>0x01 = Wakes from sleep </li></ul> |
| 5          | 0x00  | Unknown / Padding |
| 6          | 0x00  | Unknown / Padding |
| 7          | 0x**  | The button(s) being pressed (eg. LMB + RMB = 0x03)<ul><li>0x00 = No buttons pressed</li> <li>0x01 = Left click</li> <li>0x02 = Right click</li> <li>0x04 = Middle click</li> <li>0x08 = Side button back</li><li>0x10 = Side button forward</li><li>0x20 = DPI toggle button</li> |
| 8       | 0x0*  | [Shortcut function](#shortcut-functions-assignment-type--0x05) (0x00 if the button is not set to a shortcut) |

# Button Assignment Codes

## Disabled (Assignment type = 0x00)

| Code | Action   |
|------|----------|
| 0x00 | Disabled |

## Mouse functions (Assignment type = 0x01)

[USB HID Usage Tables PDF Page 102](https://www.usb.org/sites/default/files/hut1_2.pdf#page=103)

| Code | Action       |
|------|--------------|
| 0x01 | Left click   |
| 0x02 | Right click  |
| 0x03 | Middle click |
| 0x04 | Back         |
| 0x05 | Forward      |

## Keyboard functions (Assignment type = 0x02)

[USB HID Usage Tables PDF Page 82-88](https://www.usb.org/sites/default/files/hut1_2.pdf#page=83)

## Media functions (Assignment type = 0x03)

| Code | Action      |
|------|-------------|
| 0x00 | Play/Pause  |
| 0x01 | Stop        |
| 0x02 | Previous    |
| 0x03 | Next        |
| 0x04 | Volume mute |
| 0x05 | Volume down |
| 0x06 | Volume up   |

## Shortcut functions (Assignment type = 0x05)

| Code | Action |
|------|--------|
| 0x01 | Task Manager (Ctrl + Shift + Esc) |
| 0x02 | System Utility (Win + X) |
| 0x03 | Show desktop (Ctrl + D) |
| 0x04 | Cycle apps (Win + Tab) |
| 0x05 | Close window (Alt + F4) |
| 0x06 | Cut    |
| 0x07 | Copy   |
| 0x08 | Paste  |

## DPI toggle function (Assignment type = 0x07)

| Code | Action     |
|------|------------|
| 0x08 | DPI toggle |

# Miscellaneous

Extra information that doesn't belong directly in the documenation.

## Macro data sum value byte

Every [macro data packet](#macro-data) seems to have a sum byte? after the button byte that alternates between adding 1 and 2 each packet. Another way of thinking about it is half of the numbers in the sum are 1 and half are 2.

Thus, we get the following formula: $\frac{1x}{2}+\frac{2x}{2}$. Which is simplified to $\frac{3x}{2}$. Note that this is int division, so there would be no fractional part.

For example, the sum bytes for 6 packets would be: 0x00, 0x01, 0x03, 0x04, 0x06, 0x07

Now this byte probably has a completely different meaning, but it works so who cares???

## Macro event count

Generic Event = key down/up or mouse down/up<br>
Event = macro key event with up to 6 keys along with the modifier keys value or a macro mouse down / up event

The max event count in Ngenuity seems to be 80 events, because the max generic event count is also 80.  However, there is a strange bug in Ngenuity where once you reach 80 generic events, you can't add any more generic events to any macros, even new ones. This is the case until you restart Ngenuity, where you'll be able to add 1 more generic event to the macro with 80 generic events, and then the same issue occurs. However, this introduces another issue. Because 1 more generic event gets added, only the key/mouse down event is captured. This causes the button to be held down until the mouse is disconnected or asleep.

For the actual max event count, it seems to be 108 events, which is exactly 18 macro data packets. Anything beyond 108 events can produce erroneous behavior (eg. incorrect keys/buttons being sent or mouse disconnecting midway through the macro).
