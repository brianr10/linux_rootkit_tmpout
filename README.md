# linux_rootkit_tmpout
---

My rootkit for the Linux Rootkit Competition for tmp.0ut!

> Lol so I did ***not*** get to submit this in time because of work and life. But I'm still going to try to finish this because I really want to get better at writing rootkits.

## November Update
- Working through ![Singularity](https://github.com/matheuzsecurity/singularity) rootkit trying to learn how a good rootkit works. I'm copying it cause I'm still a noob at writing my own rootkit.
- I'm trying to understand the following:
    - KPROBES
        - I know there was a change in the Linux Kernel > 5.7 in exporting kernel symbols and we have to use KRPOBES. But I feel like my understanding of that is shallow.
    - LKRG Bypass
        - Linux Kernel Runtime Guard performs integrity checking of the kernel. It attempts to post-detect and respond to unauthorized modifications to the Linux kernel. This is why we have to bypass it. My problem right now is currently I would not be able to do this by myself. I have to go through Singularity line by line to understand why the LKRG is done the way that it is.
