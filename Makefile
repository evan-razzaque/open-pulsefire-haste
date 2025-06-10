CC       = gcc
CFLAGS   = $$(pkg-config --cflags gtk4) -Wall -Werror -Wno-deprecated-declarations -std=c99
LDLIBS  = -lm -lhidapi-hidraw $$(pkg-config --libs gtk4)
OBJFILES = build/buttons.o build/main.o build/mouse.o build/rgb.o 
TARGET   = bin/main

ifeq ($(OS),Windows_NT)
	LDLIBS = -lm -lhidapi $$(pkg-config --libs gtk4) -I /mingw64/include/hidapi
endif

all: $(TARGET)
	
$(TARGET) : $(OBJFILES)
	@mkdir -p bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS) 

build/buttons.o: src/buttons.c src/buttons.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main.o: src/main.c
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/mouse.o: src/mouse.c src/mouse.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/rgb.o: src/rgb.c src/rgb.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf bin build