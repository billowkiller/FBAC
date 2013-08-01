CC = gcc 
ZIP = -lz
PCAP = -lpcap
NET = -lnet
GTHREAD = -lgthread-2.0
SQLITE = -lsqlite3
QUEUE = -lnetfilter_queue
GLIB = `pkg-config --cflags --libs glib-2.0` 
OBJ = sniff.o net_util.o http_parse.o\
	  http.o data_send.o user_config.o\
	  qs_parse.o urlparser.o stringProcess.o\
	  sqlite.o

sniff:$(OBJ) 
	$(CC) -o sniff $(OBJ) $(GLIB) $(QUEUE) $(SQLITE)

sniff.o: sniff.c
	$(CC) -c $^ $(QUEUE) $(GLIB) $(SQLITE)

net_util.o: net_util.c data_send.o sqlite.o
	$(CC) -c $^ $(GLIB) $(SQLITE) 

sqlite.o: sqlite.c
	$(CC) -c  $< $(SQLITE)

http_parse.o: http_parse.c http.o qs_parse.o urlparser.o stringProcess.o
	$(CC) -c $^ $(GLIB)

http.o: http.c
	$(CC) -c  $<

urlparser.o: urlparser.c
	$(CC) -Wall -pedantic -g -c -o $@ $<

qs_parse.o: qs_parse.c
	$(CC) -c $<

stringProcess.o: stringProcess.c
	$(CC) -c $<

data_send.o: data_send.c
	$(CC) -c $< 

user_config.o: config/user_config.c
	$(CC) -c $< $(GLIB)

clean:
	rm -f sniff $(OBJ)
