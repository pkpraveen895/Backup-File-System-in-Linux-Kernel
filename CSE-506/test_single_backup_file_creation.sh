# check base functionality - to see if back up file creation works

# P.S before you do this, make sure you did ran below two commands in cd /usr/src/hw2-vyelleswarap/fs/bkpfs/
# 1) make 
# 2) sh install_module.sh 

printf "\n\nCheck base functionality - to see if back up file creation works \n\n"
make clean
make
cwd=$(pwd)
umount /mnt/bkpfs
mount -t bkpfs /test/src /mnt/bkpfs
cd /mnt/bkpfs

#remove exisiting files
rm -rf *

# create a new file, which should open a backup file 
echo a >> a.txt

#check if file got created
printf "\n\nCheck if backup file got created \n\n"
ls -la

#come back to previous path
cd $cwd


