#!/bin/bash -i

echo "Hello on PQPCS INSTALLER !"
echo
echo -e "\x1B[1;33m
  _____  _____   ____   _____  _____ 
 |  __ \|  __ \ / __ \ / ____|/ ____|
 | |__) | |__) | |  | | |    | (___  
 |  ___/|  ___/| |  | | |     \___ \ 
 | |    | |    | |__| | |___ ____) |
 |_|    |_|     \___\_\\_____|_____/ 
                       LINUX INSTALLER       
By TreesRoot\x1B[0m"
echo
echo

echo "Checking distribution ;)"
os=`uname -a | cut -d" " -f4 | cut -d'-' -f2 | cut -c 1-6`
check_buntu=`echo $os | grep -c buntu`

if [ $check_buntu -eq 0 ] ; then
   echo "your os os not *buntu distribution install by hand..."
   sleep 2
   less alternative_install.txt
   exit 0
fi

###########################################################################################################

echo "Created master directory..."
echo -n "	Enter configuration file location (/path/to/pqpcs_conf_dir): "
read choice0

if [[ `echo $choice0 | grep -c "~"` == 1 ]];then
   choice0="$HOME/`echo $choice0 | cut -c -2 --complement`"
fi
#choice0=$(echo $choice0 | sed "s/$///" # check if this is functional
mkdir "$choice0/pqpcs_conf_dir"
echo "$choice0/pqpcs_conf_dir created"

###########################################################################################################

echo "Copy files in their directory..."
mkdir $choice0/pqpcs_conf_dir/main
echo "$choice0/pqpcs_conf_dir/main created"
echo
echo "[Desktop Entry]" > $HOME/PQPCS.desktop
echo "Version=1.0" >> $HOME/PQPCS.desktop
echo "Type=Application" >> $HOME/PQPCS.desktop
echo "Name=PQPCS" >> $HOME/PQPCS.desktop
echo "Comment=Pseudo Quantic Cryptographic System launcher" >> $HOME/PQPCS.desktop
echo "Exec=$choice0/pqpcs_conf_dir/main/pqpcs.sh" >> $HOME/PQPCS.desktop
echo "Icon=" >> $HOME/PQPCS.desktop
echo "Path=" >> $HOME/PQPCS.desktop
echo "Terminal=true" >> $HOME/PQPCS.desktop
echo "StartupNotify=false" >> $HOME/PQPCS.desktop
echo "Categories=System;" >> $HOME/PQPCS.desktop
echo "$HOME/PQPCS.desktop created, moved in $HOME/.local/share/applications"
echo

mv $HOME/PQPCS.desktop $HOME/.local/share/applications/
echo "PQPCS.desktop moved !!"
echo "[Desktop Entry]" > $HOME/PQPCS-proxy.desktop
echo "Version=1.0" >> $HOME/PQPCS-proxy.desktop
echo "Type=Application" >> $HOME/PQPCS-proxy.desktop
echo "Name=PQPCS-proxy" >> $HOME/PQPCS-proxy.desktop
echo "Comment=Pseudo Quantic Cryptographic System proxy launcher" >> $HOME/PQPCS-proxy.desktop
echo "Exec=$choice0/pqpcs_conf_dir/main/pqp_proxy" >> $HOME/PQPCS-proxy.desktop
echo "Icon=" >> $HOME/PQPCS-proxy.desktop
echo "Path=" >> $HOME/PQPCS-proxy.desktop
echo "Terminal=true" >> $HOME/PQPCS-proxy.desktop
echo "StartupNotify=false" >> $HOME/PQPCS-proxy.desktop
echo "Categories=System;" >> $HOME/PQPCS-proxy.desktop
echo "$HOME/PQPCS-proxy.desktop created, moved in $HOME/.local/share/applications"
echo

mv $HOME/PQPCS-proxy.desktop $HOME/.local/share/applications/
echo "PQPCS-proxy.desktop moved !!"
echo
cp pqpcs.sh $choice0/pqpcs_conf_dir/main/
echo "pqpcs.sh copied in $choice0/pqpcs_conf_dir/main/"
echo

##########################################################################################################

bsd_ok=`dpkg-query --list libbsd-dev | grep -c ii`
if [ $bsd_ok -eq 1 ] ; then
echo "Libbsd-dev installed it's ok !"
else echo "libbsd-dev not installed, installing..."
sudo apt install libbsd-dev
echo "libbsd-dev installed, continue"
fi
echo
##########################################################################################################

echo "Compiling pqp source code and pqp proxy"
if [[ ! -e Argon2/libargon2.a ]]
cd Argon2 && make && mv libargon2.a .. && make clean && cd ..
gcc -Os -Wall -o pqp pqpcs_src.c libargon2.a -pthread -lssl -lcrypto -lbsd `pkg-config --libs libbsd`
gcc -Os -Wall -o pqp_proxy pqpcs_proxy.c libargon2.a -pthread -lssl -lcrypto -lbsd `pkg-config --libs libbsd`
mv pqp $choice0/pqpcs_conf_dir/main/
mv pqp_proxy $choice0/pqpcs_conf_dir/main/
echo "Done !"
echo

##########################################################################################################

echo "Creating PQPCS configuration file"
echo -n "	Enter matrice location (/path/to/matrice): "
read choice1
echo -n "	Enter matrice_net location (/path/to/matrice_net): "
read choice2
echo -n "	Enter table location (/path/to/table): "
read choice3
echo -n "	Enter receive file location (/path/to/pqpcs_recv_dir): "
read choice4

if [[ `echo $choice1 | grep -c "~"` -eq 1 ]];then
   choice1="$HOME/`echo $choice1 | cut -c -2 --complement`"
fi

if [[ `echo $choice2 | grep -c "~"` == 1 ]];then
   choice2="$HOME/`echo $choice2 | cut -c -2 --complement`"
fi

if [[ `echo $choice3 | grep -c "~"` -eq 1 ]];then
   choice3="$HOME/`echo $choice3 | cut -c -2 --complement`"
fi

if [[ `echo $choice4 | grep -c "~"` -eq 1 ]];then
   choice4="$HOME/`echo $choice4 | cut -c -2 --complement`"
fi

mkdir $choice4
echo "$choice4 created"
echo "Export PQPCONF & PQPRECVDIR environment variable"

export PQPCONF="$choice0/pqpcs_conf_dir"
export PQPRECVDIR="$choice4/pqpcs_recv_dir"

echo "export PQPCONF=$choice0/pqpcs_conf_dir" >> $HOME/.bashrc
echo "export PQPRECVDIR=$choice4/pqpcs_recv_dir" >> $HOME/.bashrc

echo "Done !"
echo "Writting pqpcsconf..."
echo -n "	Enter the length of the password (=0 for the default password length of 8 char.) : "
read choice5

if [[ $choice5 -eq 0 ]] ; then
	choice5=8
fi

echo "256 $choice5 1 $choice1/matrice $choice2/matrice_net $choice3/table" > $choice0/pqpcs_conf_dir/pqpcsconf
echo "// number of round to blend matrice // password lentgh // number of ip in list (to use)" >> $choice0/pqpcs_conf_dir/pqpcsconf
echo "// path to matrice // path to matrice_net // path to table" >> $choice0/pqpcs_conf_dir/pqpcsconf
echo "*EDIT THIS FILE AT YOUR OWN RISK*" >> $choice0/pqpcs_conf_dir/pqpcsconf
echo "Done !"
echo

##########################################################################################################

echo "Creating PQPCS ip list"
exec ../IP_COMPLETION/ip_search.sh

##########################################################################################################
echo "Enjoy with this program bye ;)"
echo
exit 0
