#!/bin/sh
iptables -t mangle -A PREROUTING -d www.douban.com -j DROP
