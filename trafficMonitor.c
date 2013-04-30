/*
 * =====================================================================================
 *
 *       Filename:  trafficMonitor.c
 *
 *    Description:  monitor traffic at specified device
 *    				* fliter string is defined
 *    				* loop to callback parse method
 *    				* logfile stored log.txt
 *
 *        Version:  1.0
 *        Created:  01/25/2013 03:38:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "trafficMonitor.h"

#define int_ntoa(x)	inet_ntoa(*((struct in_addr *)&x))


char http[4096];
int httplen = 0;

// struct tuple4 contains addresses and port numbers of the TCP connections
// the following auxiliary function produces a string looking like
// 10.0.0.1,1024,10.0.0.2,23
char *
adres (struct tuple4 addr)
{
  static char buf[256];
  strcpy (buf, int_ntoa (addr.saddr));
  sprintf (buf + strlen (buf), ",%i,", addr.source);
  strcat (buf, int_ntoa (addr.daddr));
  sprintf (buf + strlen (buf), ",%i", addr.dest);
  return buf;
}

void
tcp_callback (struct tcp_stream *a_tcp, void ** this_time_not_needed)
{
  char buf[1024];
  strcpy (buf, adres (a_tcp->addr)); // we put conn params into buf
  if (a_tcp->nids_state == NIDS_JUST_EST)
    {
    // connection described by a_tcp is established
    // here we decide, if we wish to follow this stream
    // sample condition: if (a_tcp->addr.dest!=23) return;
    // in this simple app we follow each stream, so..
      a_tcp->client.collect++; // we want data received by a client
      a_tcp->server.collect++; // and by a server, too
      a_tcp->server.collect_urg++; // we want urgent data received by a
                                   // server
#ifdef WE_WANT_URGENT_DATA_RECEIVED_BY_A_CLIENT
      a_tcp->client.collect_urg++; // if we don't increase this value,
                                   // we won't be notified of urgent data
                                   // arrival
#endif
      fprintf (stderr, "%s established\n\n", buf);
      return;
    }
  if (a_tcp->nids_state == NIDS_CLOSE)
    {
      // connection has been closed normally
      	fprintf (stderr, "%s closing\n\n", buf);

		//write(2,tcp_link->payload, tcp_link->datalen); // we print the newly arrived data
		//processhttp(logfile, tcp_link->payload, tcp_link->datalen);
		
      return;
    }
  if (a_tcp->nids_state == NIDS_RESET)
    {
      // connection has been closed by RST
      fprintf (stderr, "%s reset\n\n", buf);
      return;
    }

  if (a_tcp->nids_state == NIDS_DATA)
    {
      // new data has arrived; gotta determine in what direction
      // and if it's urgent or not

      struct half_stream *hlf;

      if (a_tcp->server.count_new_urg)
      {
        // new byte of urgent data has arrived 
        strcat(buf,"(urgent->)\n\n");
        buf[strlen(buf)+1]=0;
        buf[strlen(buf)]=a_tcp->server.urgdata;
        write(1,buf,strlen(buf));
        return;
      }
      // We don't have to check if urgent data to client has arrived,
      // because we haven't increased a_tcp->client.collect_urg variable.
      // So, we have some normal data to take care of.
      if (a_tcp->client.count_new)
	{
          // new data for the client
	  hlf = &a_tcp->client; // from now on, we will deal with hlf var,
      InsertNode(tcp_link, (struct iphdr *)(hlf->iphdr), (struct tcphdr *)(hlf->hdr), hlf->data, hlf->count_new);                      // which will point to client side of conn
	  
	  strcat (buf, "(<-)\n\n"); // symbolic direction of data
	}
      else
	{
	  hlf = &a_tcp->server; // analogical
	  strcat (buf, "(->)\n\n");
	  if(response_ack((struct iphdr *)(hlf->iphdr), (struct tcphdr *)(hlf->hdr)))
	  {
	  	send_direct((struct iphdr *)(hlf->iphdr), (struct tcphdr *)(hlf->hdr));
	  	DeleteNode(tcp_link, ntohl(tcphdr->ack_seq));
	  }
	}
    fprintf(stderr,"\n%s",buf); // we print the connection parameters
                              // (saddr, daddr, sport, dport) accompanied
                              // by data flow direction (-> or <-)

//nids_discard(a_tcp, 0);
    
    //write(2,hlf->data,hlf->count_new); // we print the newly arrived data
      
    }
  return ;
}

//////////////////////////////////////////////////////////////////////////////////////////

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
	int size = header->len;

	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	struct tcphdr* tcph = (struct tcphdr*)(buffer + iph->ihl*4 + sizeof(struct ethhdr));
	
	//tcp protocol
	if(IPPROTO_TCP == iph->protocol)
	{
		//printf("nids_run2");
		if(handshark(iph, tcph))
		{
			send_direct(iph, tcph);
		}
		nids_run2(buffer + sizeof(struct ethhdr), size - sizeof(struct ethhdr));
		//print_tcp_packet(buffer , size);
	}
	//printf("TCP : %d Total : %d\r", tcp, total);
}

void print_ethernet_header(const u_char *Buffer, int Size)
{
	struct ethhdr *eth = (struct ethhdr *)Buffer;

	printf("\n");
	printf("Ethernet Header\n");
	printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
	printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
	printf("   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(const u_char * Buffer, int Size)
{
	struct sockaddr_in source,dest;

	print_ethernet_header(Buffer , Size);

	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
	iphdrlen =iph->ihl*4;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;

	printf("\n");
	printf("IP Header\n");
	printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
	printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
	printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
	printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
	printf("   |-Identification    : %d\n",ntohs(iph->id));
	//printf("   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
	//printf("   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
	//printf("   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
	printf("   |-TTL      : %d\n",(unsigned int)iph->ttl);
	printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
	printf("   |-Checksum : %d\n",ntohs(iph->check));
	printf("   |-Source IP        : %s\n" , inet_ntoa(source.sin_addr) );
	printf("   |-Destination IP   : %s\n" , inet_ntoa(dest.sin_addr) );
}

void print_tcp_packet(const u_char * Buffer, int Size)
{

	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
	iphdrlen = iph->ihl*4;

	struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));

	int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

	if(Size - header_size)
    {
        printf("\n\n***********************TCP Packet*************************\n");

        print_ip_header(Buffer,Size);

        printf("\n");
        printf("TCP Header\n");
        printf("   |-Source Port      : %u\n",ntohs(tcph->source));
        printf("   |-Destination Port : %u\n",ntohs(tcph->dest));
        printf("   |-Sequence Number    : %u\n",ntohl(tcph->seq));
        printf("   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
        printf("   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
        //printf("   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
        //printf("   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
        printf("   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
        printf("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
        printf("   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
        printf("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
        printf("   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
        printf("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
        printf("   |-Window         : %d\n",ntohs(tcph->window));
        printf("   |-Checksum       : %d\n",ntohs(tcph->check));
        printf("   |-Urgent Pointer : %d\n",tcph->urg_ptr);
        printf("\n");
        printf("                        DATA Dump                         ");
        printf("\n");

        printf("Data Payload\n");
        PrintData(Buffer + header_size , Size - header_size);
		
		if(80 == (int)ntohs(tcph->source))
		{
			//printf("size:%d\n", Size - header_size);
			//processhttp(logfile, Buffer+header_size, Size-header_size);
		}
        printf("\n###########################################################");
    }
}

void PrintData (const u_char * data , int Size)
{
	int i,j;
	for(i=0 ; i < Size ; i++)
	{
		if(i%16==0) printf("\n   ");
		printf(" %02X",(unsigned int)data[i]);
	}

	printf("\n");

	for(i=0; i<Size; i++)
	{
		printf("%c",(unsigned int)data[i]);
	}
	printf(logfile, "\n");
}

void deviceChose(char* devname)
{
	pcap_if_t *alldevsp , *device;
	char errbuf[100], devs[100][100];
	int count = 1 , n;

	//First get the list of available devices
	printf("Finding available devices ... ");
	if( pcap_findalldevs( &alldevsp , errbuf) )
	{
		printf("Error finding devices : %s" , errbuf);
		exit(1);
	}
	printf("Done");

	//Print the available devices
	printf("\nAvailable Devices are :\n");
	for(device = alldevsp ; device != NULL ; device = device->next)
	{
		printf("%d. %s - %s\n" , count , device->name , device->description);
		if(device->name != NULL)
		{
			strcpy(devs[count] , device->name);
		}
		count++;
	}

	//Ask user which device to sniff
	//printf("Enter the number of the device you want to sniff : ");
	//scanf("%d" , &n);
	n=1;
	strcpy(devname,devs[n]);
	printf("devname = %s\n",devname);
	//Open the device for sniffing
	printf("Opening device %s for sniffing ... " , devname);
}

void monitor()
{
	char errbuf[100], devname[20];
	bpf_u_int32 mask;		/* Our netmask */
	bpf_u_int32 net;		/* Our IP */
	pcap_t *handle; //Handle of the device that shall be sniffed
	struct bpf_program fp;		/* The compiled filter */

	deviceChose(devname);
	printf("devname = %s\n",devname);
	/* Find the properties for the device */
	if (pcap_lookupnet(devname, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n", devname, errbuf);
		net = 0;
		mask = 0;
	}
	handle = pcap_open_live(devname , 65536 , 1 , 0 , errbuf);

	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s : %s\n" , devname , errbuf);
		exit(1);
	}

	char filter_exp[] = "port 80 and host www.douban.com";	/* The filter expression */

	/* Compile and apply the filter */
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(2);
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(2);
	}
	printf("Done\n");

	logfile=fopen("log.txt","w");
	if(logfile==NULL)
	{
		printf("Unable to create file.");
	}

	// nids_params.pcap_desc = handle;
	// nids_params.pcap_filter = filter_exp;
	// nids_params.device = devname;
	 if (!nids_init ())
	   {
	   	fprintf(stderr,"%s\n",nids_errbuf);
	   	exit(1);
	   }
	   nids_register_tcp (tcp_callback);
	   tcp_link = createNode(); 
	   //printf("%s\n", "nids_run");
	   //nids_run ();

	//Put the device in sniff loop
	pcap_loop(handle , -1 , process_packet , NULL);
}
