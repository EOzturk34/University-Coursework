all: clear compile run clean

compile: main.c 
	
	@gcc -o test main.c -lm
	
run:
	
	./test
	

clean:
	
	@rm -f *.o
	@rm -f test

clear:
	@clear
