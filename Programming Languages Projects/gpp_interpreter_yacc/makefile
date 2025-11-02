all: gpp_interpreter

gpp_interpreter: gpp_interpreter.tab.c lex.yy.c
	gcc -o gpp_interpreter gpp_interpreter.tab.c lex.yy.c -ll -I.

gpp_interpreter.tab.c: gpp_interpreter.y
	bison -d gpp_interpreter.y

lex.yy.c: gpp_interpreter.l
	flex gpp_interpreter.l

clean:
	rm -f gpp_interpreter lex.yy.c gpp_interpreter.tab.c gpp_interpreter.tab.h

