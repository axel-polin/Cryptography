#!/bin/bash -i
trap f_terminate SIGHUP SIGINT SIGTERM

f_banner(){
echo "Hello on PQPCS INSTALLER !"
echo
echo -e "\x1B[1;33m
  _____  _____   ____   _____  _____ 
 |  __ \|  __ \ / __ \ / ____|/ ____|
 | |__) | |__) | |  | | |    | (___  
 |  ___/|  ___/| |  | | |     \___ \ 
 | |    | |    | |__| | |____ ____) |
 |_|    |_|     \___\_\\_____|_____/ 
                       IP LIST COMPLETION : NETDISCOVER      
By TreesRoot\x1B[0m"
echo
echo
}

##############################################################################################################

f_terminate(){
echo "Terminating..."
sleep 0.5
clear
exit
}

##############################################################################################################

f_error(){
echo "ERROR : no arguments provided"
exit 0
}

##############################################################################################################

f_main(){

f_banner

echo -n "Enter your network interface: "
read net_if
netmask=`ifconfig $net_if | grep -i mas | awk '{print$4}' | cut -d":" -f2`
echo "Your netmask is $netmask"
ip_addr=`ifconfig $net_if | grep -i mas | awk '{print$2}' | cut -d":" -f2`
echo "Your ip address is $ip_addr"
echo
echo -n "Would you like search ip addresses (I/i) or you would use 127.0.0.1 (L/l) ?"
read choice

if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     [I,i]) echo "ok processing ip search"; f_inet;;
     [L,l]) echo "ok use 127.0.0.1 in ip_list..."; echo "127.0.0.1" > $PQPCONF/ip_list ;;
esac
echo "Done !"
}


############################################################################################################

f_inet(){

netdiscover_ok=`dpkg-query --list netdiscover | grep -c ii`
if [ $netdiscover_ok -eq 1 ] ; then
   echo "Netdiscover installed it's ok !"
   else echo "Netdiscover not installed, installing..."
   sudo apt install netdiscover
   echo "Netdiscover installed, continue"
fi
echo
echo "Running netdiscover..."

ip=`ifconfig $net_if | grep netmask | awk '{print$2}' | cut -d'.' -f1-3`
range_part1=`echo "$ip.0"`
range_part2=`cat ../IP_COMPLETION/netmask_addr | grep $netmask | awk '{print$2}'`
ip=`echo $ip | cut -d'.' -f1`

sudo netdiscover -i $net_if -r "$range_part1$range_part2" -P | awk '{print $1}' | grep $ip > $PQPCONF/ip_list
#sudo netdiscover -i $net_if -r "$range_part1$range_part2" -P | awk '{print $1,$2}' | grep $ip > $PQPCONF/ip_list
echo

}
##############################################################################################################

f_main
echo "Finish ! Enjoy with PQPCS ;)"
echo "Contact : TrEeSr00t@protonmail.com"
exit 0