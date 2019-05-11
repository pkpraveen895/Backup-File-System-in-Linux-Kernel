# check retention policy 

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make
# 2) sh install_module.sh

printf "\n\nCheck retention policy \n\n"
make clean
make
cwd=$(pwd)

umount /mnt/bkpfs
mount -t bkpfs /test/src /mnt/bkpfs
cd /mnt/bkpfs

#remove exisiting files
rm -rf *

# check if retention policy is working properly, and see if only a maximum of N = 10 ( hard coded ) backup files are created

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
echo a >> a.txt
echo a >> a.txt

#check if retention policy is working
printf "\n\n check if retention policy is working \n\n"
ls -la

#come back to previous path
cd $cwd
