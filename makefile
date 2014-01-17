CC = gcc 
ZIP = -lz
PCAP = -lpcap
NET = -lnet
GTHREAD = -lgthread-2.0
PTHREAD = -lpthread
QUEUE = -lnetfilter_queue
GLIB = `pkg-config --cflags --libs glib-2.0` 

sniff: sniff.c data_send.c session.o
	$(CC) -Wall $^ $(QUEUE) $(GLIB) -o sniff

session.o: session.c
	$(CC) -Wall -c $^ $(GLIB)
	
