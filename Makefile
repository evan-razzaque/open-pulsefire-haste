CC       = gcc
CFLAGS   = -Wall -Werror
LDLIBS  = -lhidapi-hidraw
OBJFILES = build/buttons.o build/main.o build/mouse.o build/rgb.o 
TARGET   = bin/main

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