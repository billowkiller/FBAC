#!/bin/sh

TABLE=mangle

remove_chain()
{
	echo -n removing chain...
    {
		iptables -t ${TABLE} -D PREROUTING -j NF_QUEUE_CHAIN
        iptables -t ${TABLE} -D OUTPUT -j NF_QUEUE_CHAIN
        iptables -t ${TABLE} -F NF_QUEUE_CHAIN
        iptables -t ${TABLE} -X NF_QUEUE_CHAIN
    }
    echo done
}

create_chain()
{
    echo -n creating chain...
    iptables -t ${TABLE} -N NF_QUEUE_CHAIN 
    iptables -t ${TABLE} -A NF_QUEUE_CHAIN -d 31.13.82.1 -p tcp --dport 80 -m mark --mark 0 -j NFQUEUE --queue-num 8010
    iptables -t ${TABLE} -A NF_QUEUE_CHAIN -j MARK --set-mark 0
    iptables -t ${TABLE} -I OUTPUT -j NF_QUEUE_CHAIN
	iptables -t ${TABLE} -I PREROUTING -j NF_QUEUE_CHAIN
    echo done
}

on_iqh()
{
    remove_chain
    exit 1
}

trap on_iqh INT QUIT HUP
create_chain
make
./sniff
remove_chain
