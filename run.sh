#!/bin/sh
cd /home/billowkiller/Downloads/libnids-1.24/
make clean && make && sudo make install
cd /home/billowkiller/Programmer/FBAC/
make clean && make && sudo ./sniff
