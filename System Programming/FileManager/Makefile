CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = fileManager
SRC = fileManager.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o
	
.PHONY: all clean
