all: clean run

run:
	sbcl --load loader.lisp $(FILE)

clean:
	rm -f *.fasl

.PHONY: all run clean