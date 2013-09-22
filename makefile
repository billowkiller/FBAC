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
OBJ = sniff.o net_util.o http_parse.o\
	  http.o data_send.o user_config.o\
	  qs_parse.o urlparser.o stringProcess.o\
	  sqlite.o kw_match.o flow_hash.o link.o

sniff:$(OBJ) 
	$(CC) -o sniff $(OBJ) $(GLIB) $(PCAP) $(SQLITE) $(PTHREAD)

sniff.o: sniff.c
	$(CC) -c $^ $(PCAP) $(GLIB) $(SQLITE) $(PTHREAD)
	
flow_hash.o: flow_hash.c link.o
	$(CC) -c $^ $(GLIB) $(PTHREAD)
	
link.o: link.c
	$(CC) -c $<

net_util.o: net_util.c data_send.o sqlite.o kw_match.o
	$(CC) -c $^ $(GLIB) $(SQLITE) 

sqlite.o: sqlite.c
	$(CC) -c  $< $(SQLITE)

kw_match.o: kw_match.c
	$(CC) -c  $< $(GLIB)

http_parse.o: http_parse.c http.o qs_parse.o urlparser.o stringProcess.o
	$(CC) -c $^ 

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
