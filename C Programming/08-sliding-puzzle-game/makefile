all: clear compile run clean

compile: main.c 
	
	@gcc -o test main.c -lm

run:
	
	@ulimit -s 64000 &&./test
	

clean:
	
	@rm -f *.o
	@rm -f test

clear: 
	@clear
