.PHONY: run test clean

LISP = sbcl

run: test

test:
	$(LISP) --non-interactive \
		--load prolog.lisp \
		--eval '(format t "~%=== Starting Tests ===~%")' \
		--eval '(run-tests)' \
		--eval '(terpri)' \
		--eval '(sb-ext:quit)'

clean:
	rm -f *.fasl *~