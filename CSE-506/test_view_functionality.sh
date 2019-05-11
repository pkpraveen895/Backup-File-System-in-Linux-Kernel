# check view functionality

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make
# 2) sh install_module.sh

printf "\nCheck list functionality\n\n"
make clean
make
cwd=$(pwd)

umount /mnt/bkpfs
mount -t bkpfs /test/src /mnt/bkpfs
cd /mnt/bkpfs

#remove exisiting files
rm -rf *

echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt
echo a >> a.txt

#check if files got created
printf "\n\n See existing files \n"
ls -la

#come back to previous path
cd $cwd

# give whichever version parameter you like say 11 to view it's content
printf "\nInput is : \n ./bkpctl v a.txt 11\n"

./bkpctl v a.txt 11
