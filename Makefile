CC              = gcc
LDLIBS          = -lm -lhidapi-hidraw $$(pkg-config --libs gtk4)
DEVICE_OBJFILES = build/buttons.o build/mouse.o build/rgb.o   
APP_OBJFILES    = build/main.o build/config_led.o build/config_buttons.o
OBJFILES        = $(DEVICE_OBJFILES) $(APP_OBJFILES)
TARGET          = bin/main

CFLAGS += $$(pkg-config --cflags gtk4) -Wall -Werror -Wno-deprecated-declarations -std=c23

ifeq ($(OS),Windows_NT)
	LDLIBS = -lm -lhidapi $$(pkg-config --libs gtk4) -I /mingw64/include/hidapi
endif

all: $(TARGET)
	
$(TARGET) : $(OBJFILES) 
	@mkdir -p bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS)

$(APP_OBJFILES) : src/types.h

# Device

build/buttons.o: src/device/buttons.c src/device/buttons.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/mouse.o: src/device/mouse.c src/device/mouse.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/rgb.o: src/device/rgb.c src/device/rgb.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@


# Application

build/main.o: src/main.c src/hid_keyboard_map.h src/mouse_config.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/config_led.o: src/config_led.c src/mouse_config.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/config_buttons.o: src/config_buttons.c src/mouse_config.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf bin build