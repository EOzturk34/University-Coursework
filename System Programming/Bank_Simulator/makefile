CC = gcc
CFLAGS = -Wall -Werror -g
all: bank_simulator
bank_simulator: bank_simulator.c
	$(CC) $(CFLAGS) -o bank_simulator bank_simulator.c
clean:
	rm -f bank_simulator
.PHONY: all clean
