all: libco-64 libco-32

SRC := src/*.c
INC := include/
CFLAGS += -U_FORTIFY_SOURCE

.PHONY: all

# libco only works properly under -O1
libco-64: $(SRC)
	gcc -O1 -g -shared -fPIC -m64 -I$(INC) $(SRC) -o libco-64.so

libco-32: $(SRC)
	gcc -O1 -g -shared -fPIC -m32 -I$(INC) $(SRC) -o libco-32.so

clean:
	rm -f libco*.so
