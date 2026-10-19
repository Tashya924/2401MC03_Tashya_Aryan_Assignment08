# 2401MC03_Tashya_Aryan_Assignment08
# Operating Systems Lab — Assignment 8
## Memory Management in xv6 (x86)

**Student:** Tashya Aryan  
**Roll Number:** 2401MC03  
**Repository:** [2401MC03_Tashya_Aryan_Assignment08](https://github.com/Tashya924/2401MC03_Tashya_Aryan_Assignment08)

## Overview

This assignment explores memory management in the xv6 x86 operating system. It involves virtual-to-physical address translation, physical memory statistics, page-table inspection, and lazy allocation of heap memory.

The implementation extends the xv6 kernel with new system calls and user-space test programs to demonstrate how virtual addresses map to physical memory, how physical pages are allocated and freed, how page tables represent a process's address space, and how demand paging can defer memory allocation until a page is accessed.

The assignment document provides detailed specifications for Questions 1–4. 

## Objectives

- Understand two-level paging and virtual-to-physical address translation.
- Track free physical pages and process memory usage.
- Inspect page-directory and page-table entries.
- Implement lazy allocation for heap memory.
- Handle page faults, invalid accesses, process creation, and memory cleanup.
- Test the implementation through user-space programs.

## Implemented Features

### Question 1 — Virtual-to-Physical Address Translation

**System call:** `va2pa(int va)`

The implementation translates a virtual address into its corresponding physical address by walking the calling process's page table.

Key features:
- Checks whether the page is present and accessible from user mode.
- Returns `-1` for absent or inaccessible pages.
- Prints the page-directory index, page-table index, offset, and P/W/U flags.
- Tests global variables, local variables, and dynamically allocated memory.
- Uses `fork()` to compare virtual and physical addresses between parent and child processes.

**Test program:** `vatest.c`

### Question 2 — Physical Memory Statistics

**System call:** `getmeminfo(struct meminfo *mi)`

The implementation adds physical-page accounting to the kernel allocator and exposes memory statistics to user programs.

The reported statistics include:
- Total managed physical pages.
- Free physical pages.
- Pages in the process's user address space.
- Pages used by the process's page tables.

The test examines memory usage before and after allocation and process creation, and checks whether pages are reclaimed after a child process exits and is reaped.

**Test program:** `memtest.c`

### Question 3 — Page-Table Dumping

**System call:** `vmprint(void)`

This feature prints the calling process's page directory and page-table entries in a hierarchical format.

The output includes:
- Page-directory entry indices and page-table physical addresses.
- Page-table entry indices, mapped virtual addresses, physical frame addresses, and P/W/U flags.
- Information about user-accessible mappings and the stack guard page.
- A summary of kernel entries.

The test calls `vmprint()` at startup, after growing the heap, and after shrinking it.

**Test program:** `vmprinttest.c`

### Question 4 — Lazy Heap Allocation

Lazy allocation delays physical memory allocation until a process accesses a virtual page.

The implementation modifies heap growth and page-fault handling so that:
- Positive `sbrk()` requests increase the process's address-space size without immediately allocating all requested physical pages.
- A valid page fault triggers allocation of a zeroed physical frame and maps it into the process's address space.
- Invalid memory accesses and allocation failures are handled by terminating the offending process.
- Shrinking the heap releases allocated pages.
- Process creation and kernel operations involving user buffers account for lazily allocated pages.

**Test program:** `lazytest.c`

The test requests a large heap region, accesses selected pages, checks memory usage, and exercises process and buffer-handling scenarios.

## Project Structure

```text
tashya_aryan_assignment_08/
├── README.md
├── report.pdf
├── source/
│   ├── kernel/
│   │   ├── defs.h
│   │   ├── exec.c
│   │   ├── kalloc.c
│   │   ├── meminfo.h
│   │   ├── proc.c
│   │   ├── proc.h
│   │   ├── syscall.c
│   │   ├── syscall.h
│   │   ├── sysproc.c
│   │   ├── trap.c
│   │   └── vm.c
│   ├── user/
│   │   ├── lazytest.c
│   │   ├── memtest.c
│   │   ├── vatest.c
│   │   └── vmprinttest.c
│   ├── Makefile
│   ├── user.h
│   ├── usys.S
│   └── xv6_changes.patch
├── evidence/
│   ├── q1_output.txt
│   ├── q2_output.txt
│   ├── q3_output.txt
│   ├── q4_output.txt
│   └── regression_usertests_output.txt
└── screenshots/
    └── CHECKLIST.md
```

*Note: This is the organized assignment change set. It must be used with a compatible xv6 x86 base repository to produce a complete buildable operating system. The screenshot checklist is included; actual screenshots should be added after capturing the required test runs.*

## Build and Run Instructions

### Prerequisites

- A compatible xv6 x86 source tree.
- A suitable GCC cross-compiler and supporting build tools.
- QEMU capable of running the xv6 x86 image.
- A Unix-like build environment, such as Linux or WSL.

### 1. Obtain the xv6 x86 base repository

```bash
git clone https://github.com/mit-pdos/xv6-public.git
cd xv6-public
```

If your course provides a modified xv6 x86 template, use that version instead.

### 2. Apply the changes

Copy the assignment's modified source files into their corresponding locations in the base repository. The organized `source/kernel/` and `source/user/` folders are a change set, not a complete replacement for the base tree.

Alternatively, check whether the included patch applies to your exact base revision:

```bash
git apply --check /path/to/xv6_changes.patch
```

If the check succeeds, apply it:

```bash
git apply /path/to/xv6_changes.patch
```

Do not apply both the patch and the same modified files without checking for duplicate changes.

### 3. Register the test programs

Ensure that `vatest`, `memtest`, `vmprinttest`, and `lazytest` are included in the xv6 Makefile's user-program list, following the conventions of the base repository.

Also verify that the system-call numbers, kernel dispatch table, user-space declarations, and assembly stubs are registered consistently.

### 4. Build and run

From the xv6 x86 repository root:

```bash
make clean
make
make qemu-nox
```

At the xv6 shell, run the available test programs:

```text
$ vatest
$ memtest
$ vmprinttest
$ lazytest
```

The exact commands may depend on the Makefile and emulator configuration used by your course.

## Testing and Evidence

The `evidence/` directory contains text output files associated with Questions 1–4 and a regression test run.

These files are intended to support review of:
- Virtual-to-physical address mappings.
- Physical memory usage and page reclamation.
- Page-table entries and guard-page permissions.
- Lazy allocation and page-fault handling.
- Regression behavior of existing user tests.

The recorded output should be checked against actual execution results. Capture and add genuine terminal screenshots for each question before final submission.

## Report

`report.pdf` contains the consolidated assignment report covering the implementation approach, key data structures, design decisions, challenges, and recorded observations.

Memory statistics should be interpreted according to the implementation's page-counting definitions. Any results included in the report should correspond to verified test output.

## Limitations and Notes
- The organized package contains modified files rather than every file from a complete xv6 source tree.
- Build compatibility depends on the xv6 x86 base revision and toolchain.
- The presence of text logs does not replace the requirement for genuine terminal screenshots.
- All reported test results should be verified by running the implementation in the target environment.

## Conclusion

This assignment demonstrates core operating-system memory-management mechanisms through kernel modifications and user-space tests. The work covers address translation, physical-page accounting, page-table inspection, and lazy allocation, providing practical insight into how xv6 manages process address spaces and physical memory.

---

**End of README**
