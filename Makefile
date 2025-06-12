CC       = gcc
CFLAGS   = $$(pkg-config --cflags gtk4) -Wall -Werror -Wno-deprecated-declarations -std=c99
LDLIBS  = -lm -lhidapi-hidraw $$(pkg-config --libs gtk4)
OBJFILES = build/buttons.o build/main.o build/mouse.o build/rgb.o build/config_led.o build/config_buttons.o
TARGET   = bin/main

ifeq ($(OS),Windows_NT)
	LDLIBS = -lm -lhidapi $$(pkg-config --libs gtk4) -I /mingw64/include/hidapi
endif

all: $(TARGET)
	
$(TARGET) : $(OBJFILES) 
	@mkdir -p bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS)

$(OBJFILES) : src/types.h

build/buttons.o: src/device/buttons.c src/device/buttons.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main.o: src/main.c
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/mouse.o: src/device/mouse.c src/device/mouse.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/rgb.o: src/device/rgb.c src/device/rgb.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/config_led.o: src/config_led.c src/config_led.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/config_buttons.o: src/config_buttons.c src/config_buttons.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf bin build