sudo apt-get install bison flex libnet1-dev libffi-dev zlib1g-dev gettext glib2.0-dev
sudo echo "/usr/local/lib" >> /etc/ld.so.conf
sudo ldconfig
wget http://www.tcpdump.org/release/libpcap-1.4.0.tar.gz
tar -xvf libpcap-1.4.0.tar.gz
cd libpcap-1.4.0
./configure && make && sudo make install
cd ..
rm -r libpcap-1.4.0
