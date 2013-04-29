sniff: *.c *.h
	gcc $^ -o $@ -lnids -lpcap -lnet -lgthread-2.0 -lnsl -lz

clean:
	rm -f sniff
