# check -  delete all versions

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make
# 2) sh install_module.sh

printf "\n\nCheck delete all versions \n\n"
make clean
make
cwd=$(pwd)
umount /mnt/bkpfs
mount -t bkpfs /test/src /mnt/bkpfs
cd /mnt/bkpfs

#remove exisiting files
rm -rf *

# check backup files
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

printf "\n\nSee current files\n\n"
ls -la

#come back to previous path
cd $cwd

# give version parameter as 0 to delete all version files
printf "\nInput is : \n ./bkpctl d a.txt 0 \n"

./bkpctl d a.txt 0

# go to mount point and see if file got deleted
cd /mnt/bkpfs

printf "\n\nSee current files after deletion\n\n"
ls -la

#create a new backup file
printf "\nCreate new backup file by doing an echo ( write operation )\n"
echo a >> a.txt

# check if newly created version file number is proper
printf "\n\nCheck if newly created version's file number is proper \n\n"
ls -la

#come back to previous path
cd $cwd
