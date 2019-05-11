# check delete multiple oldest versions

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make
# 2) sh install_module.sh

printf "\n\nCheck delete multiple oldest versions \n\n"
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

# give version parameter as 3,4,5,6  since it is the oldest ( version number for youngest file would be highest and version number goes down as files are more older )
# file with highest version number is youngest and file with lowest version number is oldest
# we can get oldest by going to mountpoint /mnt/bkpfs and by doing ls
# i am giving 3 as input for version number in this scenario
printf "\nInput is : \n ./bkpctl d a.txt 3 \n \n ./bkpctl d a.txt 4 \n \n ./bkpctl d a.txt 5 \n \n ./bkpctl d a.txt 6 \n"

./bkpctl d a.txt 3
./bkpctl d a.txt 4
./bkpctl d a.txt 5
./bkpctl d a.txt 6

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
