# check list functionality

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

# check if multiple backup files are created whenever you try to write
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

#check if file got created
printf "\n\n See existing files \n"
ls -la

#come back to previous path
cd $cwd

# give version parameter as 0 to delete all version files
printf "\nInput is : \n ./bkpctl l a.txt \n"

./bkpctl l a.txt 

# go to mount point and see if file got deleted
cd /mnt/bkpfs

#create a new backup file
printf "\nCreate new backup file by doing an echo ( write operation )\n"
echo a >> a.txt

#come back to previous path
cd $cwd

# check if newly created version file number is proper
printf "\n\n give list command again to see if it is working fine after creating another new backup file \n\n"
printf "\nInput is : \n ./bkpctl l a.txt \n"

./bkpctl l a.txt

cd /mnt/bkpfs
printf "\n\n See existing files \n"
ls -la

#come back to previous path
cd $cwd
