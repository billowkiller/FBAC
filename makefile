CC = gcc
LIBS = -lnids -lpcap -lnet -lgthread-2.0 -lnsl -lz
GLIB = `pkg-config --cflags --libs glib-2.0` 

sniff: *.c *.h user_config.o
	$(CC) $^ -o $@ $(LIBS) $(GLIB)

user_config.o: config/user_config.c
	$(CC) -c $^ -o $@ $(GLIB)

clean:
	rm -f sniff
