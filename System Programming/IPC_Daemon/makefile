# Makefile for IPC Program

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Program name
TARGET = ipc_program

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean compiled files
clean:
	rm -f $(TARGET) $(OBJS) fifo1 fifo2 *.log

# Run the program with test values
run: $(TARGET)
	./$(TARGET) 42 17

# Debug with GDB
debug: $(TARGET)
	gdb ./$(TARGET)

# Phony targets
.PHONY: all clean run debug
