#include <glib.h>
#include "include.h"

GSList* list = NULL;

typedef struct{
    uint32_t seq;
    int size;
}seq_unit;

int seq_register(uint32_t seq, int size)
{
    GSList *iterator = NULL;
    seq_unit* unit = (seq_unit *)malloc(sizeof(seq_unit));
    unit->seq = seq;
    unit->size = size;

    for (iterator = list; iterator; iterator = iterator->next) {
        unit->size += ((seq_unit *)iterator->data)->size;
    }

    list = g_slist_append(list, unit);
    return unit->size;
}

static int _seq_fetch(uint32_t seq)
{
    int size = 0;
    GSList *iterator = NULL;
    for (iterator = list; iterator; iterator = iterator->next) {
        if(seq > ((seq_unit *)iterator->data)->seq)
            size = ((seq_unit *)iterator->data)->size;
        else
            break;
    }
    return size;
}

int session_maintain(char **d, int from_dest)
{
	uint32_t seq = from_dest ? SEQ(TCPH(*d)) : ACK(TCPH(*d)); 
	int alenth = _seq_fetch(seq);

	//don't need modify seq or ack
	if(!alenth) return 0;

    struct tcphdr *tcph = (struct tcphdr *)TCPH(*d);

    if(from_dest)
        tcph->seq = htonl(SEQ(tcph)+alenth);
    else
        tcph->ack_seq = htonl(ACK(tcph)-alenth);
    
	return 1;
}

//int main()
//{
//    seq_register(1000, 20);
//    seq_register(2000, 10);
//    assert(0 == seq_fetch(100));
//    assert(20 == seq_fetch(1000));
//    seq_register(3000, 20);
//    assert(30 == seq_fetch(2000));
//	assert(50 == seq_fetch(2500));
//	assert(50 == seq_fetch(4000));
//	printf("test ok!!!\n");
//}
