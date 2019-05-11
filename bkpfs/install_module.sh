#!/bin/sh
set -x
# WARNING: this script doesn't check for errors, so you have to enhance it in case any of the commands
# below fail.
#lsmod
umount /mnt/bkpfs
rmmod bkpfs
insmod bkpfs.ko
mount -t bkpfs /test/src /mnt/bkpfs
lsmod

