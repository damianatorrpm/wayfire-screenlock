all: libscreenlock.so

libscreenlock.so: screenlock.o
	cc -shared -o $@ screenlock.o `pkg-config --libs wayfire`

screenlock.o: screenlock.cpp
	cc -Wall -c -fPIC -o $@ `pkg-config --cflags wayfire` -I/usr/include/pixman-1 -DWLR_USE_UNSTABLE -DWAYFIRE_PLUGIN screenlock.cpp

clean:
	-rm -f *.o *.so *~
