#!/bin/bash -i

echo "Hello on PQPCS MAT&TAB GENERATOR !"
echo
echo -e "\x1B[1;33m
  _____  _____   ____   _____  _____ 
 |  __ \|  __ \ / __ \ / ____|/ ____|
 | |__) | |__) | |  | | |    | (___  
 |  ___/|  ___/| |  | | |     \___ \ 
 | |    | |    | |__| | |____ ____) |
 |_|    |_|     \___\_\\_____|_____/ 
                       MATRICE and TABLE GENERATOR      
By TreesRoot\x1B[0m"
echo
echo -n "Would you like generate Table (T/t) or Matrice (M/m) or check matrice (C/c) ?"
read choice
echo 
echo "Checking files..."
file=`ls -a | grep -c GEN_MAT`
if [ "$file" != 2 ] ; then
echo "GEN_MAT not found compiling source code..."
gcc -pedantic -Os -Wall -o GEN_MAT GEN_MAT.c libargon2.a -pthread -lssl -lcrypto -lbsd `pkg-config --libs libbsd`
fi
file=`ls -a | grep -c GEN_TABLE`
if [ "$file" != 2 ] ; then
echo "GEN_TABLE not found compiling source code..."
gcc -pedantic -Os -Wall -o GEN_TABLE GEN_TABLE.c libargon2.a -pthread -lssl -lcrypto -lbsd `pkg-config --libs libbsd`
fi
echo "Done !"
sleep 1
case $choice in 
     [M,m]) echo "Generating matrice..." ; ./GEN_MAT > matrice_gen.txt;;
     [T,t]) echo "Generating matrice..." ; ./GEN_TABLE > table_gen.txt;;
     [F,f]) echo "Checking matrice" ; for i in `seq 0 255` ; do 
	if [[ `cat matrice_gen.txt | grep -c "$i"` == '0' ]] ; then
	echo "Number not found matrice not complete erase file please..."
	cat matrice_gen.txt | grep -c "$i"
	exit 0
	fi
	echo "$i ok switch..."
	done;;
     *) echo "Invalid choice, exiting..." ; exit 0
esac