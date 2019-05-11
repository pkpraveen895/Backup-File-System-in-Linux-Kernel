# check if delete youngest versions one after the other works

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make
# 2) sh install_module.sh

printf "\n\nCheck if delete youngest versions one after the other works\n\n"
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

printf "\n\nSee current files\n\n"
ls -la

#come back to previous path
cd $cwd

# give version parameter as since it is the youngest ( version number for youngest file would be highest and version number goes down as files are more older
# file with highest version number is youngest and file with lowest version number is oldest
# we can get youngest by going to mountpoint /mnt/bkpfs and by doing ls
# i am giving 10,9,8,7 as inputs for version number in this scenario
printf "\nInput is : \n ./bkpctl d a.txt 10 \n ./bkpctl d a.txt 9\n ./bkpctl d a.txt 8\n ./bkpctl d a.txt 7\n"

./bkpctl d a.txt 10
./bkpctl d a.txt 9
./bkpctl d a.txt 8
./bkpctl d a.txt 7

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
