# check base functionality - to see if multiple back up files are created properly

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make
# 2) sh install_module.sh

printf "\nCheck base functionality - to see if multiple back up files are created properly \n\n"
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

#check if file got created
printf "\n\nCheck if backup files got created \n"
ls -la

#come back to previous path
cd $cwd
