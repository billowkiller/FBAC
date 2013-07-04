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

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
	int size = header->len;
	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	
	//tcp protocol
	
	char *ip = "192.168.1.250";
	char *hostname = "www.douban.com";
	if(IPPROTO_TCP == iph->protocol && ishost(iph, hostname))//isFromSrc(iph, ip))
	{
		switch(tcp_type(iph))
		{
			case FIRSTSHARK:
			case THIRDSHARK:
				//send_data((char *)iph, SEND_DIRECT);
				break;
			case GET:
				if(!content_filter(iph))
				{
					printf("GET\n");
					//print_tcp_packet(buffer , size);
					//send_data((char *)iph, SEND_GET);
				}
				break;
			case POST:
				printf("POST\n");
				print_tcp_packet(buffer , size);
			default:
				;
				//send_data((char *)iph, SEND_DIRECT);
		}

		//nids_run2(buffer + sizeof(struct ethhdr), size - sizeof(struct ethhdr));
		//print_tcp_packet(buffer , size);
	}
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

	char filter_exp[] = "port 80";	/* The filter expression */

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

	//Put the device in sniff loop
	pcap_loop(handle , -1 , process_packet , NULL);
}
