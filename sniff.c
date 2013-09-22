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

#include "sniff.h"

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
	int size = header->len;
	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	
	if(IPPROTO_TCP == iph->protocol)
	{
		//send_data((char *)iph, SEND_UP);
		store_data((char *)iph);
	}
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
char filter_exp[] = "src host 211.147.4";
	//char filter_exp[] = "dst host 173.252.110 or dst host 31.13.82 or dst host 220.181.181";	/* The filter expression */

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
 
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_sqlite
 *  Description:  
 * =====================================================================================
 */
int init_sqlite()
{
    char* dbpath="/home/wutao/FBAC/config/fbac.db";
    char *zErrMsg = 0;
    int rc;
    //open the database file.If the file is not exist,it will create a file.
    rc = sqlite3_open(dbpath, &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
        return 0;
    }
	return 1;
}		/* -----  end of function init_sqlite  ----- */
int main()
{
	//init_sqlite();
	//pipe_config();
	monitor();
}
