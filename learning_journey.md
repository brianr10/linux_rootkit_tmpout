# What I Learned
---

## Capability
### Basic Function Hooking
- Initially I didn't understand why we have to use function pointers, especially when it came to function hooking.

### Becoming Root Capability
```
We have to hook the `kill` linux command ...
```

## Things I do ***NOT*** understand
- ~~Why we use `kprobe`.~~
- `within_module()` function. I have never seen this before.
- spinlock & why we use it in this context (I forgot my OS class material over the past 4 years lol).
- This line of code: `orig_register_kprobe = (int (*)(struct kprobe*))addr`.
- Not sure what `iomem` is.
- wtf is `copy_mc_to_kernel`?
- Why we use `pt_regs()`

## Things I Learned
- We use `kprobe` because:
    - Kprobes enables you to dynamically break into any kernel routine and collect debugging and performance information non-disruptively. You can trap at almost any kernel code address, specifying a handler routine to be invoked when the breakpoint is hit. [7]
    - We need to kprobes for our rootkit so we can overwrite system call table entries or patch function prologues. Temporary kprobes are a way to recover symbol addresses or instrument a live kernel. (I keep seeing 'instrument' often. 'Instrument' a binary for fuzzing, 'instrument' a live kernel.) According to stack overflow, to 'instrument' a program means to modify it so you can observe, measure or control its behavior.

# References
1. https://phrack.org/issues/73/the-art-of-staying-hidden_md#article
2. https://tmpout.sh/4/10.html
3. https://sysprog21.github.io/lkmpg/
4. https://github.com/MatheuZSecurity/Singularity
5. https://xcellerator.github.io/posts/linux_rootkits_02/
6. https://h0mbre.github.io/Learn-C-By-Creating-A-Rootkit/
7. https://docs.kernel.org/trace/kprobes.html

###### Quickly realizing I know nothing about rootkits...
