# Makefile for Multithreaded Log File Analyzer
# CSE 344 - Homework #4

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpthread

# Target executable
TARGET = LogAnalyzer

# Source files
SOURCES = main.c buffer.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Header files
HEADERS = buffer.h

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files
main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

buffer.o: buffer.c $(HEADERS)
	$(CC) $(CFLAGS) -c buffer.c

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean