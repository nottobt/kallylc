# KallyLC (Linux Crash Simulation Utility)

---

## 📌 Overview

**KallyLC** is an experimental system-testing utility designed for Linux systems. It allows developers and kernel engineers to test and develop several error codes, and several other bugs to test.

---

> ## ⚠️ DISCLAIMER: SYSTEM INSTABILITY AND DATA LOSS

> Running **KallyLC**, particularly Option 4, may destabilize, crash, or panic your operating system's kernel.

> **Unsaved data may be lost.** Some operations may cause system-wide disruption or require a reboot.

> Run KallyLC only in a disposable virtual machine or other isolated test environment. Do not run it on a production system or a machine containing important data.

> The author and contributors provide this software without warranty, subject to the terms of the GNU Affero General Public License, version 3.0 or later (AGPL-3.0-or-later). They disclaim liability to the extent permitted by applicable law. See the accompanying `LICENSE` file for the complete license terms.

---

## 🚀 Features

* **MCE Hardware Corruption (`MADV_HWPOISON`):** Flags simulated defective RAM pages in user-space to test kernel Machine Check Exception handling.
* **General Protection Fault (#GP):** Dereferences non-canonical virtual addresses (`0xFFFF6FFFFFFFFFFF`) to provoke memory management unit (MMU) faults.
* **User-Space Null Pointer Dereference:** Triggers clean segmentation faults (`SIGSEGV`).
* **Dynamic Kernel Module Generator:** Automatically builds dynamic Makefiles to compile and load kernel-level bugs
  * `kallylclkm1.ko` – Kernel Warning (`WARN()`)
  * `kallylclkm2.ko` – Kernel Bug (`BUG()`)
  * `kallylclkm3.ko` – Kernel Oops via Null Pointer Dereference
  * `kallylclkm4.ko` – Kernel Panic (`panic()`)
  * `kallylclkm5.ko` – Kernel preemption & Out of memory (OOM) w/ High nice
* **Memory leaker** Leaks memory via memory allocation.
* **Fork bomb** Forks multiple processes in order to lock up the Linux system.
* **File descriptor exhaustion** Exhausts the file descriptor (May be subject to file descriptor limits (ulimit)
* **Buffer overflow** Overflows the buffer via arrays.
* **Secure Boot Check:** Built-in parser for EFI variables to verify Secure Boot status before attempting kernel module insertions.

* Prerequisites
Ensure you have the essential C toolchain and Linux kernel headers installed:
```bash
# On Debian/Ubuntu systems:
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r) git
```

Compile the executable.
```bash
git clone https://github.com/nottobt/kallylc.git
cd kallylc
gcc kallylc.c -O3 -I. -o kallylc
```
There are multiple flags for gcc.
   -DDEBUG - Enters the program in debug mode.
   -DPROT - Protected mode - refuses to execute anything and exits.
Run this with root privileges 
```bash
sudo ./kallylc
``` 
