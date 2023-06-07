test: test.c file-cow.h
	gcc -o test test.c
	./test

clean:
	rm -f test
