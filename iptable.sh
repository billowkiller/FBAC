#!/bin/sh
sudo iptables -t mangle -A PREROUTING -d 14.0.63.0/24 -j DROP
sudo iptables -t mangle -A PREROUTING -d 175.41.12.68/24 -j DROP
sudo iptables -t mangle -A PREROUTING -d 220.181.181.0/24 -j DROP
