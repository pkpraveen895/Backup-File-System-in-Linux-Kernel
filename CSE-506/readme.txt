Design decisions :

1) Size of any file created must be less than 1024 bytes

2) A backup file is created just before a write operation is performed on any file.

2) In retention policy, value of maximum number of files is N (hard coded to 10 ), version number is appended to the file while creating a backup file.

Smallest version number means it is oldest, and largest version number represents youngest.

Suppose if we already have 10 back up files, when 11th back up file is created, 1st file will be deleted. for example if we have

1 a.txt (oldest), 2 a.txt, 3 a.txt, 4 a.txt, 5 a.txt, 6 a.txt, 7 a.txt, 8 a.txt, 9 a.txt, 10 a.txt (youngest)

when 11th file is created it becomes

11 a.txt(youngest), 2 a.txt(oldest), 3 a.txt, 4 a.txt, 5 a.txt, 6 a.txt, 7 a.txt, 8 a.txt, 9 a.txt, 10 a.txt

when 12th file is created it becomes

11 a.txt, 12 a.txt(youngest), 3 a.txt(oldest), 4 a.txt, 5 a.txt, 6 a.txt, 7 a.txt, 8 a.txt, 9 a.txt, 10 a.txt

2) In restore functionality when a file ( xyz.txt ) is restored using a backup version a new file will be opened with the name ( restored_xyz.txt ) and all back up versions of the file will be deleted


Note - Visibility policy is not implemented, so it is better to directly go to mount point to see if any operations performed are done properly instead of using a list command


Kernel Code ( is in in /usr/src/hw2-vyellleswarap/fs/bkpfs/ )
------------------------------------------------------------------

Bkpfs module is built on top of wrapfs ( the wrapfs code is taken and changes are made on top of it ).

Bkpfs is a loadable module so please do make ( which will give us the .ko file ) and run "sh install_module.sh" in bkpfs directory to load the module and also to mount the file system

before running any of the user commands, run below two commands in /usr/src/hw2-vyellleswarap/fs/bkpfs/

1) make
2) sh install_module.sh     - this command will unmount /mnt/bkpfs/ , install the module and then mount it the file system.

1) Back up file creation

Whenever a wrote operation is about to be performed on a file, before it's done we create a new negative upper dentry and and allocate memory to it and also do a lookup to see if it has lower dentry ( which it will not ) then we also create a lower dentry, for the newly created upper dentry and perform a do splice direct operation to copy the existing content in the file to the file corresponding to the newly created dentry.

2) Retention policy

If more than N(10) backup files for a particular file are already created, then we delete the X - N backup file by using vfs_unlink, here X being the next version number ( for X = 11, we delete 1st back up file and so on )

we again do a back up file creation with version number as X as mentioned above. Through out retntion policy we carefully update the current oldest and youngest version names using vfs_setxattr.

3) IOCTL code

code was updated in bkpfs_unlocked_ioctl, these are the command numbers used for the operations 1000 - delete, 1001 - restore, 1002 - view , 1003 - list

     ->  In delete, we check if he wants to delete all versions or just one version(oldest or youngest). If it is oldest or youngest, we use d_lookup and delete ( which calls vfs_unlink ) which will delete the file. If the request is delete all, we dome the same d_lookup and delete but for all back up version files names between youngest and oldest ( inclusive )

     ->  In restore
          1) we save it's file name and delete the file using d_lookup and delete ( which calls vfs_unlink )
          2) we create a restored file by starting with a new negative dentry and do same steps as in back up file creation, only difference being that the newly created file's name will have "restored_" in front of it
          3) we copy the content to restored file by using the version number given to us, using a do_splice_direct
          4) we then remove all the back up version file names between youngest and oldest

       -> In view, we perform a kernel_read operation over the backup version file and store the bytes and set it to struct query q and do a copy_to_user command

       -> In list, we get the youngest and oldest slab numbers by doing a vfs_getxattr and update these values to youngest and oldest in struct query q and do a copy_to_user.



User Code ( is in /usr/src/hw2-vyelleswarap/CSE506 )
----------------------------------------------------

P.S - before running any of the below two commands in /usr/src/hw2-vyellleswarap/fs/bkpfs/
1) make
2) sh install_module.sh - this command will unmount /mnt/bkpfs/ , install the module and then mount it. All the below commands will only work when first it is mounted. So make sure it is mounted.

Also do a "make clean", "make" in the CSE506 directory before running the commands

Usage - Syntax is:
./bkpctl d | r | v <input_file> <version number>
./bkpctl l <input_file>

A help function ./bkpctl h is also added to check the syntax

1) d -> delete operation

    Delete operation takes input file, and version number of the back up file
    version number of back up file must be youngest/oldest backup file's version number

    Example -> ./bkpctl d a.txt 3

    To check the youngest or oldest version number you can go to the mount point
     cd /mnt/bkpfs

     Alternatively version number can also be 0 if you want to delete all files

     Example -> ./bkpctl d a.txt 0

 2) r -> restore operation

      restore operation takes input file, and version number of any back up file

      After restore operation is performed a new file with name ( restored_xxxx.txt) will be created and also all the backup versions will be deleted

In both delete and restore operations we check error scenario's and carefully take command line parameters and set them to struct query q and pass it to kernel using ioctl

        Example -> ./bkpctl r a.txt 3

  3)  v -> view operation

     view operation takes input file, and version number of any back up file
     the object of struct query q which is passed to kernel using ioctl, will be updated using copy_to_user in kernel.
     We display the buffer from query q, which we stored the file content.

         Example -> ./bkpctl v a.txt 3

  4)   l  -> list operation

     list operation takes input file
     the object of struct query q which is passed to kernel using ioctl, will be updated using copy_to_user in kernel.
     We display the existing versions using youngest and oldest values from query q, which we stored the file content.

         Example ->  ./bkpctl l a.txt


