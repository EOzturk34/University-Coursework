all: clear clean compile run

compile: main.c util.c util.h

	@gcc -o test main.c util.c -lm

run:
	./test

clean:
	@rm -f *.o
	@rm -f test

clear:
	@clear
