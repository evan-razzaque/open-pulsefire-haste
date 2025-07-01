CC              = gcc
LDLIBS          = -lm -lhidapi-hidraw $$(pkg-config --libs gtk4)
DEVICE_OBJFILES = build/buttons.o build/mouse.o build/rgb.o build/sensor.o
APP_OBJFILES    = build/main.o build/config_led.o build/config_buttons.o build/config_macro.o build/config_sensor.o
OBJFILES        = $(DEVICE_OBJFILES) $(APP_OBJFILES)
UI_FILES        = ui/templates.gresource.xml ui/dpi-profile-config.ui
GRESOURCES      = resources/templates.gresource
TARGET          = bin/main

CFLAGS += $$(pkg-config --cflags gtk4) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99 -O1

ifeq ($(OS),Windows_NT)
	LDLIBS = -lm -lhidapi $$(pkg-config --libs gtk4) -I /mingw64/include/hidapi
endif

all: $(TARGET) $(GRESOURCES) 
	
$(TARGET) : $(OBJFILES)
	@mkdir -p bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS)

$(GRESOURCES) : $(UI_FILES)
	@mkdir -p resources
	glib-compile-resources $< --sourcedir ui --target $@

$(APP_OBJFILES) : src/types.h src/mouse_config.h

# Device

build/%.o: src/device/%.c src/device/%.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@


# Application

build/main.o : src/main.c src/hid_keyboard_map.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/%.o: src/%.c
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm -rf bin build resources