INC := ../include/

.PHONY: test libco

all: libco-test-64 libco-test-32

test: libco all
	@echo "==== TEST 64 bit ===="
	@LD_LIBRARY_PATH=.. ./libco-test-64
	@echo "==== TEST 32 bit ===="
	@LD_LIBRARY_PATH=.. ./libco-test-32

libco:
	@cd .. && make

libco-test-64: libco main.c
	gcc -g -I$(INC) -L.. -m64 main.c -o libco-test-64 -lco-64

libco-test-32: libco main.c
	gcc -g -I$(INC) -L.. -m32 main.c -o libco-test-32 -lco-32

clean:
	rm -f libco-test-*
