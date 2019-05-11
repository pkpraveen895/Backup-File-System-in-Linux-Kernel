/*
 * Copyright (c) 1998-2017 Erez Zadok
 * Copyright (c) 2009	   Shrikar Archak
 * Copyright (c) 2003-2017 Stony Brook University
 * Copyright (c) 2003-2017 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "bkpfs.h"
#include "bkpctl.h"
#define   N        10

static int create(struct inode *dir, struct dentry *dentry,
                         umode_t mode, bool want_excl)
{
        int err;
        struct dentry *lower_dentry;
        struct dentry *lower_parent_dentry = NULL;
        struct path lower_path;

        bkpfs_get_lower_path(dentry, &lower_path);
        lower_dentry = lower_path.dentry;
        lower_parent_dentry = lock_parent(lower_dentry);

        err = vfs_create(d_inode(lower_parent_dentry), lower_dentry, mode,
                         want_excl);
        if (err)
                goto out;
       
        fsstack_copy_attr_times(dir, bkpfs_lower_inode(dir));
        fsstack_copy_inode_size(dir, d_inode(lower_parent_dentry));

out:
        unlock_dir(lower_parent_dentry);
        bkpfs_put_lower_path(dentry, &lower_path);
        return err;
}

static int delete(struct inode *dir, struct dentry *lower_dentry)
{
        int err;
        struct inode *lower_dir_inode = bkpfs_lower_inode(dir);
        struct dentry *lower_dir_dentry;
      
        dget(lower_dentry);
        lower_dir_dentry = lock_parent(lower_dentry);

        err = vfs_unlink(lower_dir_inode, lower_dentry, NULL);

        if (err == -EBUSY && lower_dentry->d_flags & DCACHE_NFSFS_RENAMED)
                err = 0;
        if (err)
                goto out;
        fsstack_copy_attr_times(dir, lower_dir_inode);
        fsstack_copy_inode_size(dir, lower_dir_inode);

out:
        unlock_dir(lower_dir_dentry);
        dput(lower_dentry);
        return err;
}

static ssize_t bkpfs_read(struct file *file, char __user *buf,
			   size_t count, loff_t *ppos)
{
	int err;
	struct file *lower_file;
	struct dentry *dentry = file->f_path.dentry;

	lower_file = bkpfs_lower_file(file);
	err = vfs_read(lower_file, buf, count, ppos);
	/* update our inode atime upon a successful lower read */
	if (err >= 0)
		fsstack_copy_attr_atime(d_inode(dentry),
					file_inode(lower_file));

	return err;
}

static ssize_t bkpfs_write(struct file *file, const char __user *buf,
			    size_t count, loff_t *ppos)
{
        int err;
        struct file *lower_file;
        struct qstr this;
        struct qstr this1;
        struct dentry *parent;
        struct dentry *new_dentry;
        struct dentry *lower_dentry;
        struct dentry *lower_dir_dentry = NULL;
        struct path lower_parent_path;
        struct path *lower_parent_path1;
        //char buffer[1024]; 
        struct file *lower_file_src = NULL;
        struct path lower_src_path;
        struct file *lower_file_dest = NULL;
        struct path lower_dest_path;
        int bytes = 1;

        char name[100];
        char name1[100];

        int val;
        void *p = &val;
        size_t size = 20;      
        const char rev[14] = "user.revision";
        const char *revision = rev;        
        const char young[14] = "user.youngest";
        const char *youngest = young;
        struct dentry *dentry = file->f_path.dentry;

        // file creation starts here
        vfs_getxattr(dentry,revision,p,size);
        if(val < 0 )
		val = 1;
 	else
                val = val + 1;

        vfs_setxattr(dentry,revision,p,size,0);
        vfs_setxattr(dentry,youngest,p,size,0);        

        sprintf( name, "%d %s", val,dentry->d_name.name);
        parent = dget_parent(dentry);

        this.name = name;
        this.len = strlen(name);
        this.hash = full_name_hash(parent, this.name, this.len);
 
        // retention policy code starts here     
        if(val>N)
        {
            sprintf( name1, "%d %s", (val-N),dentry->d_name.name);
   
            bkpfs_get_lower_path(parent, &lower_parent_path);
            lower_parent_path1 = &lower_parent_path;
            lower_dir_dentry = lower_parent_path1->dentry;
 
            this1.name = name1;
            this1.len = strlen(name1);
           
            this1.hash = full_name_hash(lower_dir_dentry, this1.name, this1.len);        
            lower_dentry = d_lookup(lower_dir_dentry, &this1);
            
            if(lower_dentry)
            {
                err = delete(d_inode(dget_parent(dentry)),lower_dentry);
                if (err)
                    return err;
            }
        }
        // retention policy code ends here


        new_dentry = d_alloc(parent, &this);
        if (!new_dentry) {
                err = -ENOMEM;
                goto out;
        }
    
        lower_dentry = bkpfs_lookup(d_inode(parent), new_dentry, LOOKUP_CREATE);
        err = create(d_inode(parent), new_dentry, dentry->d_inode->i_mode, true);
        if (err)
                 return err;
        // file creation ends here

        // file copy starts here
        bkpfs_get_lower_path(dentry, &lower_src_path);
        lower_file_src = dentry_open(&lower_src_path, O_RDONLY, current_cred());

        if ( !lower_file_src || IS_ERR(lower_file_src))
        {
                err = PTR_ERR(lower_file_src);
                goto out;
        }

        bkpfs_get_lower_path(new_dentry, &lower_dest_path);
        lower_file_dest = dentry_open(&lower_dest_path, O_WRONLY, current_cred());
        
        if ( !lower_file_dest ||  IS_ERR(lower_file_dest))
        {
                err = PTR_ERR(lower_file_dest);
                goto out;
        }

         while( bytes > 0 )
       {
           bytes = do_splice_direct(lower_file_src, &lower_file_src->f_pos, lower_file_dest, &lower_file_dest->f_pos, 4096, 0);
       }
out:
        if (err)
                return err;
        // file copy ends here

       // write code
        lower_file = bkpfs_lower_file(file);
        err = vfs_write(lower_file, buf, count, ppos);
        // update our inode times+sizes upon a successful lower write 
                if (err >= 0) {
                fsstack_copy_inode_size(d_inode(dentry),
                                        file_inode(lower_file));
                fsstack_copy_attr_times(d_inode(dentry),
                                        file_inode(lower_file));
        }

        return err;

}

static int bkpfs_readdir(struct file *file, struct dir_context *ctx)
{
	int err;
	struct file *lower_file = NULL;
	struct dentry *dentry = file->f_path.dentry;

	lower_file = bkpfs_lower_file(file);
	err = iterate_dir(lower_file, ctx);
	file->f_pos = lower_file->f_pos;
	if (err >= 0)		/* copy the atime */
		fsstack_copy_attr_atime(d_inode(dentry),
					file_inode(lower_file));
	return err;
}

static long bkpfs_unlocked_ioctl(struct file *file, unsigned int cmd,
				  unsigned long arg)
{
	long err = -ENOTTY;
	struct file *lower_file;
        struct query *q;
        struct query q2;
        void *p2 = &q2;
        int i;
        char name[100];
        char name1[100];
        char name2[100];
        char name3[100];
        char *buffer = NULL;
        struct qstr this;
        struct qstr this1;
        struct qstr this2;
        struct dentry *parent;
        struct dentry *lower_dentry;
        struct dentry *lower_dir_dentry = NULL;
        struct path lower_parent_path;
        struct path *lower_parent_path1;
        struct path lower_dest_path;
        struct path lower_src_path;
        struct file *lower_file_dest = NULL;
        int val;
        void *p = &val;
        int val1;
        void *p1 = &val1;
        size_t size = 20;
        const char res[10]="restored_";
        const char *restored = res;
        const char rev[14] = "user.revision";
        const char *revision = rev;
        const char young[14] = "user.youngest";
        const char *youngest = young;
        struct dentry *dentry = file->f_path.dentry;
        struct dentry *new_dentry;
        struct vfsmount *lower_dir_mnt;
        struct file *lower_file_src = NULL;
        int bytes = 1;
        
        buffer =(char *) kmalloc(1024 * sizeof(char),GFP_KERNEL);
	lower_file = bkpfs_lower_file(file);        
       
	/* XXX: use vfs_ioctl if/when VFS exports it */
	if (!lower_file || !lower_file->f_op)
		goto out;

        q = ( struct query *)arg;
        
        // code to switch as per ioctl commands
        switch (cmd)
        {
           // delete ioctl
           case 1000:
               
               // delete oldest or youngest
               if(q->version != 0)
               {
                    parent = dget_parent(dentry);            
                    sprintf( name, "%d %s", q->version,&q->name[11]);
                    printk(" revision file - %s",name);
         
                    bkpfs_get_lower_path(parent, &lower_parent_path);
                    lower_parent_path1 = &lower_parent_path;
                    lower_dir_dentry = lower_parent_path1->dentry;
                
                    this.name = name;
                    this.len = strlen(name);
                    this.hash = full_name_hash(lower_dir_dentry, this.name, this.len);
                    lower_dentry = d_lookup(lower_dir_dentry, &this);

                    if(lower_dentry)
                    {
                        err = delete(d_inode(dget_parent(dentry)),lower_dentry);
                        if (err)
                            return err;

                        vfs_getxattr(dentry,youngest,p,size);
                        if(q->version == val)
                        {
                            val1 = val - 1;
                            vfs_setxattr(dentry,revision,p1,size,0);
                            vfs_setxattr(dentry,youngest,p1,size,0);
                        }
                    }
                }
                else //delete all
                {
                    vfs_getxattr(dentry,revision,p,size);
 
                    for( i = val; i > 0; i--)
                    {
                         parent = dget_parent(dentry);
                         sprintf( name, "%d %s", i,&q->name[11]);

                         bkpfs_get_lower_path(parent, &lower_parent_path);
                         lower_parent_path1 = &lower_parent_path;
                         lower_dir_dentry = lower_parent_path1->dentry;

                         this.name = name;
                         this.len = strlen(name);
                         this.hash = full_name_hash(lower_dir_dentry, this.name, this.len);
                         lower_dentry = d_lookup(lower_dir_dentry, &this);

                         if(lower_dentry)
                         {
                             err = delete(d_inode(dget_parent(dentry)),lower_dentry);
                             if (err)
                                 return err;
                         } 
                    }

                    val1 = -1;
                    vfs_setxattr(dentry,revision,p1,size,0);
                }

           break;

           // restore backup file ioctl
           case 1001:
                sprintf( name1, "%s", &q->name[11]);
                
                //delete current file
                parent = dget_parent(dentry);
                bkpfs_get_lower_path(parent, &lower_parent_path);
                lower_parent_path1 = &lower_parent_path;
                lower_dir_dentry = lower_parent_path1->dentry;
 
                this.name = name1;
                this.len = strlen(name1);
                this.hash = full_name_hash(lower_dir_dentry, this.name, this.len);
                lower_dentry = d_lookup(lower_dir_dentry, &this);

                if(lower_dentry)
                {
                    err = delete(d_inode(dget_parent(dentry)),lower_dentry);
                    if (err)
                        return err;
                }

               
               // create restored file using a negative dentry
               sprintf( name2, "%s%s",restored,name1);
               this1.name = name2;
               this1.len = strlen(name2);
               this1.hash = full_name_hash(lower_dir_dentry, this1.name, this1.len);
               
               new_dentry = d_alloc(parent, &this1);
               if (!new_dentry) {
                    err = -ENOMEM;
                    goto out;
               }

               lower_dentry = bkpfs_lookup(d_inode(parent), new_dentry, LOOKUP_CREATE);
               err = create(d_inode(parent), new_dentry, dentry->d_inode->i_mode, true);
               if (err)
                    return err;

               // copy content to restored file
               sprintf( name3, "%d %s", q->version,name1);

               parent = dget_parent(dentry);
               this2.name = name3;
               this2.len = strlen(name3);
               this2.hash = full_name_hash(parent, this2.name, this2.len);
              
               lower_dir_mnt = lower_parent_path1->mnt;

               err = vfs_path_lookup(lower_dir_dentry, lower_dir_mnt, name3, 0,
                              &lower_src_path);
               if(err)
                   return err;

               lower_file_src = dentry_open(&lower_src_path, O_RDONLY, current_cred());

               if ( !lower_file_src || IS_ERR(lower_file_src))
               {
                     err = PTR_ERR(lower_file_src);
                     goto out;
               }

               bkpfs_get_lower_path(new_dentry, &lower_dest_path);
               lower_file_dest = dentry_open(&lower_dest_path, O_WRONLY, current_cred());

               if ( !lower_file_dest ||  IS_ERR(lower_file_dest))
               {
                    err = PTR_ERR(lower_file_dest);
                    goto out;
               }

               while( bytes > 0 )
               {
                    bytes = do_splice_direct(lower_file_src, &lower_file_src->f_pos, lower_file_dest, &lower_file_dest->f_pos, 4096, 0);
               }

               // file copy ends here

               //remove all remaining versions
               vfs_getxattr(dentry,revision,p,size);

               for( i = val; i > 0; i--)
               {
                     parent = dget_parent(dentry);
                     sprintf( name, "%d %s", i,&q->name[11]);

                     bkpfs_get_lower_path(parent, &lower_parent_path);
                     lower_parent_path1 = &lower_parent_path;
                     lower_dir_dentry = lower_parent_path1->dentry;

                     this.name = name;
                     this.len = strlen(name);
                     this.hash = full_name_hash(lower_dir_dentry, this.name, this.len);
                     lower_dentry = d_lookup(lower_dir_dentry, &this);

                     if(lower_dentry)
                     {
                          err = delete(d_inode(dget_parent(dentry)),lower_dentry);
                          if (err)
                               return err;
                     }
               }
 
           break;
           
           // view content of backup file - ioctl command
           case 1002:
                parent = dget_parent(dentry);
                sprintf( name, "%d %s", q->version,&q->name[11]);

                parent = dget_parent(dentry);
                
                this2.name = name;
                this2.len = strlen(name);
                this2.hash = full_name_hash(parent, this2.name, this2.len);
                
                bkpfs_get_lower_path(parent, &lower_parent_path);
                lower_parent_path1 = &lower_parent_path;
                lower_dir_dentry = lower_parent_path1->dentry;
                lower_dir_mnt = lower_parent_path1->mnt;

                err = vfs_path_lookup(lower_dir_dentry, lower_dir_mnt, name, 0,
                              &lower_src_path);
                if(err)
                   return err;

                lower_file_src = dentry_open(&lower_src_path, O_RDONLY, current_cred());
          
                if ( !lower_file_src || IS_ERR(lower_file_src))
                {
                     err = PTR_ERR(lower_file_src);
                     goto out;
                }

                printk(" file name - %s",lower_file_src->f_path.dentry->d_name.name);
                
                bytes =  kernel_read(lower_file_src, buffer,1024, &lower_file_src->f_pos);
                if( bytes < 0) 
                {
                    err = bytes;
                    printk(" reading input from version failed\n");
                    goto out;
                 }

                 sprintf( q2.buff, "%s", buffer);
                
                 if ( copy_to_user( ( struct query *)arg , p2 , sizeof(struct query) ) )
                 {
                     return -EACCES;
                 }
                 err = 0; 
           break;

           case 1003:

              vfs_getxattr(dentry,revision,p,size);
              q2.youngest = val;
              
               if( val <= N)
                   q2.oldest = 1;
               else
                   q2.oldest = val1 - N;

               if ( copy_to_user( ( struct query *)arg , p2 , sizeof(struct query) ) )
               {
                    return -EACCES;
               }

                err = 0;

            break;


           default:
	       if (lower_file->f_op->unlocked_ioctl)
	   	   err = lower_file->f_op->unlocked_ioctl(lower_file, cmd, arg);

	        /* some ioctls can change inode attributes (EXT2_IOC_SETFLAGS) */
	        if (!err)
		    fsstack_copy_attr_all(file_inode(file),
				      file_inode(lower_file));
        }
out:
	return err;
}

#ifdef CONFIG_COMPAT
static long bkpfs_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	long err = -ENOTTY;
	struct file *lower_file;

	lower_file = bkpfs_lower_file(file);

	/* XXX: use vfs_ioctl if/when VFS exports it */
	if (!lower_file || !lower_file->f_op)
		goto out;
	if (lower_file->f_op->compat_ioctl)
		err = lower_file->f_op->compat_ioctl(lower_file, cmd, arg);

out:
	return err;
}
#endif

static int bkpfs_mmap(struct file *file, struct vm_area_struct *vma)
{
	int err = 0;
	bool willwrite;
	struct file *lower_file;
	const struct vm_operations_struct *saved_vm_ops = NULL;

	/* this might be deferred to mmap's writepage */
	willwrite = ((vma->vm_flags | VM_SHARED | VM_WRITE) == vma->vm_flags);

	/*
	 * File systems which do not implement ->writepage may use
	 * generic_file_readonly_mmap as their ->mmap op.  If you call
	 * generic_file_readonly_mmap with VM_WRITE, you'd get an -EINVAL.
	 * But we cannot call the lower ->mmap op, so we can't tell that
	 * writeable mappings won't work.  Therefore, our only choice is to
	 * check if the lower file system supports the ->writepage, and if
	 * not, return EINVAL (the same error that
	 * generic_file_readonly_mmap returns in that case).
	 */
	lower_file = bkpfs_lower_file(file);
	if (willwrite && !lower_file->f_mapping->a_ops->writepage) {
		err = -EINVAL;
		printk(KERN_ERR "bkpfs: lower file system does not "
		       "support writeable mmap\n");
		goto out;
	}

	/*
	 * find and save lower vm_ops.
	 *
	 * XXX: the VFS should have a cleaner way of finding the lower vm_ops
	 */
	if (!BKPFS_F(file)->lower_vm_ops) {
		err = lower_file->f_op->mmap(lower_file, vma);
		if (err) {
			printk(KERN_ERR "bkpfs: lower mmap failed %d\n", err);
			goto out;
		}
		saved_vm_ops = vma->vm_ops; /* save: came from lower ->mmap */
	}

	/*
	 * Next 3 lines are all I need from generic_file_mmap.  I definitely
	 * don't want its test for ->readpage which returns -ENOEXEC.
	 */
	file_accessed(file);
	vma->vm_ops = &bkpfs_vm_ops;

	file->f_mapping->a_ops = &bkpfs_aops; /* set our aops */
	if (!BKPFS_F(file)->lower_vm_ops) /* save for our ->fault */
		BKPFS_F(file)->lower_vm_ops = saved_vm_ops;

out:
	return err;
}

static int bkpfs_open(struct inode *inode, struct file *file)
{
	int err = 0;
	struct file *lower_file = NULL;
	struct path lower_path;

	/* don't open unhashed/deleted files */
	if (d_unhashed(file->f_path.dentry)) {
		err = -ENOENT;
		goto out_err;
	}

	file->private_data =
		kzalloc(sizeof(struct bkpfs_file_info), GFP_KERNEL);
	if (!BKPFS_F(file)) {
		err = -ENOMEM;
		goto out_err;
	}

	/* open lower object and link bkpfs's file struct to lower's */
	bkpfs_get_lower_path(file->f_path.dentry, &lower_path);
	lower_file = dentry_open(&lower_path, file->f_flags, current_cred());
	path_put(&lower_path);
	if (IS_ERR(lower_file)) {
		err = PTR_ERR(lower_file);
		lower_file = bkpfs_lower_file(file);
		if (lower_file) {
			bkpfs_set_lower_file(file, NULL);
			fput(lower_file); /* fput calls dput for lower_dentry */
		}
	} else {
		bkpfs_set_lower_file(file, lower_file);
	}

	if (err)
		kfree(BKPFS_F(file));
	else
		fsstack_copy_attr_all(inode, bkpfs_lower_inode(inode));
out_err:
	return err;
}

static int bkpfs_flush(struct file *file, fl_owner_t id)
{
	int err = 0;
	struct file *lower_file = NULL;

	lower_file = bkpfs_lower_file(file);
	if (lower_file && lower_file->f_op && lower_file->f_op->flush) {
		filemap_write_and_wait(file->f_mapping);
		err = lower_file->f_op->flush(lower_file, id);
	}

	return err;
}

/* release all lower object references & free the file info structure */
static int bkpfs_file_release(struct inode *inode, struct file *file)
{
	struct file *lower_file;

	lower_file = bkpfs_lower_file(file);
	if (lower_file) {
		bkpfs_set_lower_file(file, NULL);
		fput(lower_file);
	}

	kfree(BKPFS_F(file));
	return 0;
}

static int bkpfs_fsync(struct file *file, loff_t start, loff_t end,
			int datasync)
{
	int err;
	struct file *lower_file;
	struct path lower_path;
	struct dentry *dentry = file->f_path.dentry;

	err = __generic_file_fsync(file, start, end, datasync);
	if (err)
		goto out;
	lower_file = bkpfs_lower_file(file);
	bkpfs_get_lower_path(dentry, &lower_path);
	err = vfs_fsync_range(lower_file, start, end, datasync);
	bkpfs_put_lower_path(dentry, &lower_path);
out:
	return err;
}

static int bkpfs_fasync(int fd, struct file *file, int flag)
{
	int err = 0;
	struct file *lower_file = NULL;

	lower_file = bkpfs_lower_file(file);
	if (lower_file->f_op && lower_file->f_op->fasync)
		err = lower_file->f_op->fasync(fd, lower_file, flag);

	return err;
}

/*
 * Bkpfs cannot use generic_file_llseek as ->llseek, because it would
 * only set the offset of the upper file.  So we have to implement our
 * own method to set both the upper and lower file offsets
 * consistently.
 */
static loff_t bkpfs_file_llseek(struct file *file, loff_t offset, int whence)
{
	int err;
	struct file *lower_file;

	err = generic_file_llseek(file, offset, whence);
	if (err < 0)
		goto out;

	lower_file = bkpfs_lower_file(file);
	err = generic_file_llseek(lower_file, offset, whence);

out:
	return err;
}

/*
 * Bkpfs read_iter, redirect modified iocb to lower read_iter
 */
ssize_t
bkpfs_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	int err;
	struct file *file = iocb->ki_filp, *lower_file;

	lower_file = bkpfs_lower_file(file);
	if (!lower_file->f_op->read_iter) {
		err = -EINVAL;
		goto out;
	}

	get_file(lower_file); /* prevent lower_file from being released */
	iocb->ki_filp = lower_file;
	err = lower_file->f_op->read_iter(iocb, iter);
	iocb->ki_filp = file;
	fput(lower_file);
	/* update upper inode atime as needed */
	if (err >= 0 || err == -EIOCBQUEUED)
		fsstack_copy_attr_atime(d_inode(file->f_path.dentry),
					file_inode(lower_file));
out:
	return err;
}

/*
 * Bkpfs write_iter, redirect modified iocb to lower write_iter
 */
ssize_t
bkpfs_write_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	int err;
	struct file *file = iocb->ki_filp, *lower_file;

	lower_file = bkpfs_lower_file(file);
	if (!lower_file->f_op->write_iter) {
		err = -EINVAL;
		goto out;
	}

	get_file(lower_file); /* prevent lower_file from being released */
	iocb->ki_filp = lower_file;
	err = lower_file->f_op->write_iter(iocb, iter);
	iocb->ki_filp = file;
	fput(lower_file);
	/* update upper inode times/sizes as needed */
	if (err >= 0 || err == -EIOCBQUEUED) {
		fsstack_copy_inode_size(d_inode(file->f_path.dentry),
					file_inode(lower_file));
		fsstack_copy_attr_times(d_inode(file->f_path.dentry),
					file_inode(lower_file));
	}
out:
	return err;
}

const struct file_operations bkpfs_main_fops = {
	.llseek		= generic_file_llseek,
	.read		= bkpfs_read,
	.write		= bkpfs_write,
	.unlocked_ioctl	= bkpfs_unlocked_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= bkpfs_compat_ioctl,
#endif
	.mmap		= bkpfs_mmap,
	.open		= bkpfs_open,
	.flush		= bkpfs_flush,
	.release	= bkpfs_file_release,
	.fsync		= bkpfs_fsync,
	.fasync		= bkpfs_fasync,
	.read_iter	= bkpfs_read_iter,
	.write_iter	= bkpfs_write_iter,
};

/* trimmed directory options */
const struct file_operations bkpfs_dir_fops = {
	.llseek		= bkpfs_file_llseek,
	.read		= generic_read_dir,
	.iterate	= bkpfs_readdir,
	.unlocked_ioctl	= bkpfs_unlocked_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= bkpfs_compat_ioctl,
#endif
	.open		= bkpfs_open,
	.release	= bkpfs_file_release,
	.flush		= bkpfs_flush,
	.fsync		= bkpfs_fsync,
	.fasync		= bkpfs_fasync,
};
