#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#include <linux/namei.h>

#include "ftrace_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skilledPaRaDoX");
MODULE_DESCRIPTION("mkdir syscall hook");
MODULE_VERSION("0.01");

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif

#ifdef PTREGS_SYSCALL_STUBS
/*
 * What is `asmlinkage`?*/
static asmlinkage long (*orig_mkdir)(const struct pt_regs *);

asmlinkage int hook_mkdir(const struct pt_regs *regs) {
  char __user *pathname = (char *)regs->di;
  char dir_name[NAME_MAX] = { 0 };

  long error = strncpy_from_user(dir_name, pathname, NAME_MAX);

  if (error > 0)
      printk(KERN_INFO "[!] Rootkit: Trying to create directory w/ name: %s\n", dir_name);

  orig_mkdir(regs);
  return 0;
}
#else
static asmlinkage long (*orig_mkdir)(const char __user *pathname, umode_t mode);

asmlinkage int hook_mkdir(const char __user *pathname, umode_t mode) {
  char dir_name[NAME_MAX] = { 0 };
  
  long error = strncpy_from_user(dir_name, pathname, NAME_MAX);

  if (error > 0)
      printk("[!] Rootkit: Trying to create directory w/ name %s\n", dir_name);

  orig_mkdir(pathname, mode);
  return 0;
}
#endif

static int __init example_init(void) {
    // Print string to kernel buffer
    // see message w/ dmesg. You always start w/
    // KERN_* macro which defines log level
    printk(KERN_INFO "Hello, world!\n");
    return 0;
}

static void __exit example_exit(void) {
    printk(KERN_INFO "Goodbye, world!\n");
}

// This declares to the compiler the roles
// of the following functions
module_init(example_init);
module_exit(example_exit);
