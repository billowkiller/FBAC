CC = gcc 
ZIP = -lz
PCAP = -lpcap
NET = -lnet
GTHREAD = -lgthread-2.0
PTHREAD = -lpthread
SQLITE = -lsqlite3
QUEUE = -lnetfilter_queue
GLIB = `pkg-config --cflags --libs glib-2.0` 
PYTHON = -I/usr/include/python2.6/ -L/usr/lib -lpython2.6

sniff: sniff.c data_send.c
	$(CC) $^ $(QUEUE) -o sniff
