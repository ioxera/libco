all: libco-64 libco-32

CFLAGS += -U_FORTIFY_SOURCE

.PHONY: all

# libco only works properly under -O1
libco-64: co.c
	gcc -O1 -g -shared -fPIC -m64 co.c -o libco-64.so

libco-32: co.c
	gcc -O1 -g -shared -fPIC -m32 co.c -o libco-32.so

clean:
	rm -f libco*.so
