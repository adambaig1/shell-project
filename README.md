# xv6-riscv OS Enhancements & Custom Shell

This repository contains my extensions to xv6-riscv, a modern ANSI C re-implementation of Dennis Ritchie and Ken Thompson's classic Unix Version 6. Originally developed for teaching at MIT, this version runs on an emulated RISC-V multiprocessor architecture.

---

## Software Overview

This project extends the base xv6 operating system with several new user-space programs and kernel-level modifications:

* **Custom Shell (`my_shell.c`)**: A fully functional command-line interface that supports input/output redirection (`>`, `<`), multi-element pipelines (`|`), and sequential command execution (`;`).


* **System Calls**: Extended the kernel by adding new system calls, including `getyear`, which interacts directly with the kernel to return OS metadata.


* **User Utilities**: Implemented some Unix utilities in C for testing inter-process communication via pipes.

---

## Project Architecture
```text
.
├── xv6-riscv/
│   ├── kernel/           # Monolithic OS kernel (processes, memory, traps, fs)
│   ├── user/             # Unprivileged user-space applications and libraries
│   │   ├── my_shell.c    # Custom command-line shell implementation
│   │   └── ...           # Other utilities (grep, cat, echo, pingpong)
│   ├── mkfs/             # Utility to build the initial file system image (fs.img)
│   └── Makefile          # Build automation and QEMU orchestration
├── README.md             # You are here!
└── setup.sh              # Setup file to install pre-requisites
```

---

## Getting Started

Because the build system relies on Unix command-line utilities, **you must use a Unix-like environment** (macOS, Linux, or WSL 2 on Windows) to run this project. 

This repository includes an automated setup script that detects your operating system and installs the necessary RISC-V GNU toolchain and QEMU emulator.

1. **Install Dependencies**:
From the root of the repository, run the setup script:
```bash
bash setup.sh
```

2. **Build and Boot the OS**:
In the `xv6-riscv/` directory of the project, compile the system and launch the emulator by running:
```bash
cd xv6-riscv
make clean
make qemu
```
Note: This will build the kernel, generate the file system, and drop you into the default xv6 `sh` shell.

3. **Launch the Custom Shell**:
Once inside the emulator, start the custom shell by typing:
```bash
my_shell
```
You will see the `>>>` prompt, indicating the custom shell is active and ready for complex pipeline commands.

4. **Exit the Emulator**:
To safely quit QEMU, press `Ctrl-A`, release both keys, and then press `X`.