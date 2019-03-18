#!/bin/sh

hexchat -e PQPCS-IRC #run hexchat with PQPCS-IRC

hexchat_port=`sudo netstat -ntp | grep hexchat | cut -d' ' -f33 | cut -d'/' -f1` #search the TCP port using by hexchat.
sudo iptables -t nat -A PREROUTING -p tcp --dport $hexchat_port -j REDIRECT --to-ports 2061 #redirect traffic from the hexchat port on PQPCS proxy port (2061).
./pqp_proxy
echo "Done !"