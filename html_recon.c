#include "html_recon.h"

// struct tuple4 contains addresses and port numbers of the TCP connections
// the following auxiliary function produces a string looking like
// 10.0.0.1,1024,10.0.0.2,23
char *
adres (struct tuple4 addr)
{
  static char buf[256];
  strcpy (buf, int_ntoa (addr.saddr));
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
		//FreeLink(tcp_link);
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
        //write(1,buf,strlen(buf));
        return;
      }
      // We don't have to check if urgent data to client has arrived,
      // because we haven't increased a_tcp->client.collect_urg variable.
      // So, we have some normal data to take care of.
      if (a_tcp->client.count_new)
	{
          // new data for the client
	  hlf = &a_tcp->client; // from now on, we will deal with hlf var,
      //InsertNode(tcp_link, (struct iphdr *)(hlf->iphdr), (struct tcphdr *)(hlf->tcphdr), hlf->data, hlf->count_new);                      // which will point to client side of conn
	  
	  strcat (buf, "(<-)\n\n"); // symbolic direction of data
	}
      else
	{
	  hlf = &a_tcp->server; // analogical
	  strcat (buf, "(->)\n\n");
	  write(2,hlf->data,hlf->count_new);
	  //if(response_ack((struct iphdr *)(hlf->iphdr), (struct tcphdr *)(hlf->tcphdr)))
	  {
	  	//send_direct((struct iphdr *)(hlf->iphdr), (struct tcphdr *)(hlf->hdr));
	  	//DeleteNode(tcp_link, ntohl(tcphdr->ack_seq));
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

void store_html(char *data, int len)
{
 	if (!nids_init ())
    {
   		exit(1);
    }
    nids_register_tcp (tcp_callback);
   // tcp_link = createNode(); 
	nids_run2(data, len);
}