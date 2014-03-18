CC = gcc
all:
	$(CC) -Wall -O3 -o emitter emitter.c `pkg-config --cflags --libs glib-2.0 gio-unix-2.0`
	$(CC) -Wall -O3 -o receiver receiver.c `pkg-config --cflags --libs glib-2.0 gio-unix-2.0`	

clean:
	rm -f emitter receiver *.o
	
