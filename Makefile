CC       = gcc
CFLAGS   = -Wall -Werror
LDFLAGS  = -lhidapi-hidraw
OBJFILES = build/*.o
TARGET   = bin/main

VPATH    = src

all: dirs $(TARGET)

.PHONY: dirs
dirs:
	mkdir -p build bin

$(TARGET) : $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -rf bin build