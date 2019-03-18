#!/bin/bash -i

# Catch process termination
trap f_terminate SIGHUP SIGINT SIGTERM

medium='==============================================================='

sip='sort -n -u -t . -k 1,1 -k 2,2 -k 3,3 -k 4,4'

##############################################################################################################

f_banner(){
echo
echo -e "\033[1;33m
  _____  _____   ____   _____  _____ 
 |  __ \|  __ \ / __ \ / ____|/ ____|
 | |__) | |__) | |  | | |    | (___  
 |  ___/|  ___/| |  | | |     \___ \ 
 | |    | |    | |__| | |____ ____) |
 |_|    |_|     \___\_\\_____|_____/ v.10.0
                       AUTOMATIC LAUNCHER       
By TreesRoot\x1B[0m"
echo
echo
}

##############################################################################################################

f_error(){
echo
echo -e "\033[1;31m$medium\033[0m"
echo
echo -e "\033[1;31m           *** Invalid choice or entry. ***\033[0m"
echo
echo -e "\033[1;31m$medium\033[0m"
sleep 2
f_main
}

##############################################################################################################

f_terminate(){
echo "Terminating..."
clear
exit
}

##############################################################################################################
f_encrypt_0(){
echo -e "\033[1;34mENCRYPT MODE 0\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo "1.  Run on normal mode;"
echo "2.  Run with recursive mode;"
echo
echo
echo -n "Choice: "
read choice

# Check for no answer
if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     1) $PQPCONF/main/pqp encrypt 0;;
     2) $PQPCONF/main/pqp encrypt 0 -r;;
     *) f_error;;
esac
}

##############################################################################################################
f_encrypt_1(){
echo -e "\033[1;34mENCRYPT MODE 1\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo "1.  Run on normal mode;"
echo "2.  Run with recursive mode;"
echo
echo
echo -n "Choice: "
read choice

# Check for no answer
if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     1) $PQPCONF/main/pqp encrypt 1;; #$PQPCONF/main/pqp encrypt 0;;
     2) $PQPCONF/main/pqp encrypt 1 -r;; #$PQPCONF/main/pqp encrypt 0 -r;;
     *) f_error;
esac
}

##############################################################################################################
f_encrypt_3(){
echo -e "\033[1;34mENCRYPT INTERACTIVE NET MODE\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo "1.  Run with recursive mode;"
echo "2.  Run with chat mode;"
echo "3.  Run on normal mode;"
echo -n "Choice: "
read choice
echo
echo -n "IP Destination: "
read dest_ip

# Check for no answer
if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     1) $PQPCONF/main/pqp encrypt 3 -r $dest_ip;;
     2) $PQPCONF/main/pqp encrypt 3 -c $dest_ip;;
     3) $PQPCONF/main/pqp encrypt 3 $dest_ip;;
     *) f_error;;
esac
}

##############################################################################################################
f_decrypt_0(){
echo -e "\033[1;34mDECRYPT MODE 0\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo "1.  Run on normal mode;"
echo "2.  Run with recursive mode;"
echo
echo
echo -n "Choice: "
read choice

# Check for no answer
if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     1) $PQPCONF/main/pqp decrypt 0;;
     2) $PQPCONF/main/pqp decrypt 0 -r;;
     *) f_error;;
esac
}

##############################################################################################################
f_decrypt_1(){
echo -e "\033[1;34mDECRYPT MODE 1\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo "1.  Run on normal mode;"
echo "2.  Run with recursive mode;"
echo
echo
echo -n "Choice: "
read choice

# Check for no answer
if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     1) $PQPCONF/main/pqp decrypt 1;;
     2) $PQPCONF/main/pqp decrypt 1 -r;;
     *) f_error;;
esac
}

##############################################################################################################
f_decrypt_2(){
echo -e "\033[1;34mDECRYPT MODE 2\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo "1.  Run with direct display;"
echo "2.  Run with display redirection in file;"
echo
echo
echo -n "Choice: "
read choice

# Check for no answer
if [[ -z $choice ]]; then
     f_error
fi

case $choice in
     1) $PQPCONF/main/pqp decrypt 2 2;;
     2) $PQPCONF/main/pqp decrypt 2 1;;
     *) f_error;;
esac
}

##############################################################################################################
f_help(){
echo "This is the help part of pqpcs"
echo "SYNOPSIS AND HOW IT WORKS"
echo
echo "It exist 2 main mode : encrypt and decrypt"
echo
echo "In encrypt mode your are 4 secondary mode : 0 1 2 3"
echo
echo "	--> Mode 0 is used to encrypt file or directory localy with your own file password and your own local matrice"
echo "	--> Mode 1 is used to encrypt with remote matrice and your own password file"
echo "	--> Mode 2 is used to share your network matrice for the utilisation of mode 1"
echo "	--> Mode 3 is used to share encrypted file or directory or to chat with a remote computer"
echo
echo "In decrypt mode you are 3 secondary mode : 0 1 2"
echo
echo "	--> Mode 0 is used to decrypt file or directory localy with your own file password and your own local matrice"
echo "	--> Mode 1 is used to decrypt with remote matrice and your own password file"
echo "	--> Mode 2 is used to display the file content shared by other computer with you (or recieve message in chat mode) or to save recieved file in save directory"
echo
echo "FULL EXAMPLES"
echo
echo "Encrypt mode and mode 0 : $PQPCONF/main/pqp encrypt 0 [-r if you encrypt directory]"
echo "Encrypt mode and mode 1 : $PQPCONF/main/pqp encrypt 1 [-r if you encrypt directory]"
echo "Encrypt mode and mode 2 : $PQPCONF/main/pqp encrypt 2"
echo "Encrypt mode and mode 3 : $PQPCONF/main/pqp encrypt 3 [-r (if you share directory)  | -c (if you start chat mode)] dest_ip"
echo
echo "Decrypt mode and mode 0 : $PQPCONF/main/pqp decrypt 0 [-r if you decrypt directory]"
echo "Decrypt mode and mode 1 : $PQPCONF/main/pqp decrypt 1 [-r if you decrypt directory]"
echo "Decrypt mode and mode 2 display mode 1: $PQPCONF/main/pqp decrypt 2 1 (save file or message in save directory)"
echo "Decrypt mode and mode 2 display mode 2: $PQPCONF/main/pqp decrypt 2 2 (display message or file content in terminale)"
}

##############################################################################################################
f_custom(){
echo -n "Enter your own pqpcs command or bash command here: "
read exec_command

# Check for no answer
if [[ -z $exec_command ]]; then
     f_error
fi

echo " Executing..."
$exec_command
clear
}

##############################################################################################################
f_kill(){
echo "Kill pqp daemon and pqp_proxy daemon if exists"
killall -15 pqp && killall -15 pqp_proxy
sleep 2
echo  "Done !"
f_terminate
}

##############################################################################################################
f_proxy(){
echo "Running proxy ;) : `$PQPCONF/main/pqp_proxy`"
}

##############################################################################################################

f_main(){
#clear
f_banner
num=`ps -aux | grep "pqp encrypt 2" | awk 'END {print NR}'`

if [[ -z $PQPCONF ]];then
   echo "PQPCONF environment variable empty error exiting (3 seconds)"
   sleep 3
   f_terminate
elif [[ -z $PQPRECVDIR ]];then
   echo "PQPRECVDIR environment variable empty error exiting (3 seconds)"
   sleep 3
   f_terminate
fi

if [[ $num -eq 1 ]]; then
	$PQPCONF/main/pqp encrypt 2
fi

echo -e "\033[1;34mENCRYPT\033[0m"    # In MacOS X, using \x1B instead of \e. \033 would be ok for all platforms.
echo
echo "1.  Run on local mode"
echo "2.  Run on net mode"
echo "3.  Run on net interactive mode"
echo
echo -e "\033[1;34mDECRYPT\033[0m"
echo
echo "4.  Run on local mode"
echo "5.  Run on net mode"
echo "6.  Run on net interactive mode"
echo
echo
echo "10. Exit"
echo "11. Help"
echo "12. Custom command (at your own risk)"
echo "13. Kill daemons"
echo "14. Run Proxy"
echo
echo -n "Choice: "
read choice

case $choice in
     1) f_encrypt_0;;
     2) f_encrypt_1;;
     3) f_encrypt_3;;
     4) f_decrypt_0;;
     5) f_decrypt_1;;
     6) f_decrypt_2;;
     10) clear && exit;;
     11) f_help;;
     12) f_custom;;
     13) f_kill;;
     14) f_proxy;;
     *) f_error;;
esac
}

##############################################################################################################

while true; do f_main; done
exit 0
