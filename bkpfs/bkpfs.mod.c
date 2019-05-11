#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x114b11bc, "module_layout" },
	{ 0x49225d4e, "vfs_create" },
	{ 0x2b7cf930, "kmem_cache_destroy" },
	{ 0xd0c8109b, "kmalloc_caches" },
	{ 0xed85d97d, "fsstack_copy_inode_size" },
	{ 0x450bebf5, "generic_file_llseek" },
	{ 0xd649ce08, "mntget" },
	{ 0x44ce44d7, "inode_permission" },
	{ 0x754d539c, "strlen" },
	{ 0x6d706907, "d_set_d_op" },
	{ 0x1033bfb5, "iget5_locked" },
	{ 0x3ba5586c, "__generic_file_fsync" },
	{ 0x6742abbb, "dget_parent" },
	{ 0xb8b043f2, "kfree_link" },
	{ 0x97e21ac0, "vfs_link" },
	{ 0x8ee5ad0d, "generic_fh_to_parent" },
	{ 0x340882b9, "filemap_write_and_wait" },
	{ 0xd0e36a0b, "touch_atime" },
	{ 0x154cc51d, "generic_delete_inode" },
	{ 0xd9b85ef6, "lockref_get" },
	{ 0x95b34f82, "dput" },
	{ 0xe6844d07, "dentry_open" },
	{ 0x63f2e0c0, "vfs_path_lookup" },
	{ 0xc66d8b14, "vfs_mknod" },
	{ 0x1d8b35b5, "d_add" },
	{ 0x934e6914, "generic_read_dir" },
	{ 0x787c90ea, "igrab" },
	{ 0x7d8d3173, "vfs_symlink" },
	{ 0x81517b31, "mount_nodev" },
	{ 0xc5e6b6e, "path_get" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x828757a, "truncate_setsize" },
	{ 0x3d40734e, "vfs_rmdir" },
	{ 0xfc4c477c, "unlock_rename" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xa34edcca, "kernel_read" },
	{ 0xb34cb68e, "vfs_read" },
	{ 0x69a2acad, "kern_path" },
	{ 0x5471cb5, "current_task" },
	{ 0xc5850110, "printk" },
	{ 0x65de4113, "d_rehash" },
	{ 0x46608d89, "vfs_getxattr" },
	{ 0x3e787d7a, "kmem_cache_free" },
	{ 0x80f9d478, "lock_rename" },
	{ 0x44b9f3e5, "set_nlink" },
	{ 0x374650b7, "up_write" },
	{ 0x949382f, "down_write" },
	{ 0x7eed3510, "fput" },
	{ 0xc74c160e, "inode_init_once" },
	{ 0xb4e0d7df, "kmem_cache_alloc" },
	{ 0xde27577b, "ilookup" },
	{ 0x78c70b1c, "d_alloc" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x9d6c2af5, "unlock_new_inode" },
	{ 0xd00404c4, "d_drop" },
	{ 0xdb7f079c, "inode_newsize_ok" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0x2c10debd, "vfs_statfs" },
	{ 0xdbf26a2f, "vfs_mkdir" },
	{ 0xd6eaaea1, "full_name_hash" },
	{ 0xe6e9c0b3, "path_put" },
	{ 0xc8371a60, "kmem_cache_alloc_trace" },
	{ 0xdbf17652, "_raw_spin_lock" },
	{ 0x237e33fb, "vfs_unlink" },
	{ 0x48e96108, "kmem_cache_create" },
	{ 0x91f3bb49, "register_filesystem" },
	{ 0x80b69906, "fsstack_copy_attr_all" },
	{ 0x378d533a, "d_lookup" },
	{ 0xe082c6af, "do_splice_direct" },
	{ 0xc8e72371, "iput" },
	{ 0x37a0cba, "kfree" },
	{ 0xecd301cb, "d_splice_alias" },
	{ 0x7faaf5d, "d_make_root" },
	{ 0xf2d58902, "iterate_dir" },
	{ 0xf4095a3c, "unregister_filesystem" },
	{ 0xcab33697, "init_special_inode" },
	{ 0xaec1836f, "vfs_rename" },
	{ 0x8e990632, "vfs_getattr" },
	{ 0x2e83d534, "generic_fh_to_dentry" },
	{ 0x9cb0e130, "notify_change" },
	{ 0x4afdd491, "vfs_setxattr" },
	{ 0x6c92e662, "clear_inode" },
	{ 0x7b967ccb, "vfs_listxattr" },
	{ 0xa176595d, "clear_nlink" },
	{ 0x51065ce6, "vfs_removexattr" },
	{ 0x627c4dbe, "vfs_write" },
	{ 0x4ea821aa, "vfs_fsync_range" },
	{ 0x9466b5a6, "setattr_prepare" },
	{ 0x7d0ad062, "generic_fillattr" },
	{ 0xe914e41e, "strcpy" },
	{ 0x8cf6b358, "truncate_inode_pages" },
	{ 0x72fa428b, "generic_shutdown_super" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "AFA36543FFCA398AD67C252");
