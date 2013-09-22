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
OBJ = sniff.o net_util.o \
	  data_send.o flow_hash.o link.o

sniff:$(OBJ) 
	$(CC) -o sniff $(OBJ) $(GLIB) $(PCAP) $(PTHREAD)

sniff.o: sniff.c
	$(CC) -c $^ $(PCAP) $(GLIB) $(PTHREAD)
	
flow_hash.o: flow_hash.c link.o
	$(CC) -c $^ $(GLIB) $(PTHREAD)
	
link.o: link.c
	$(CC) -c $<

net_util.o: net_util.c
	$(CC) -c $^

data_send.o: data_send.c
	$(CC) -c $< 

clean:
	rm -f sniff $(OBJ)
