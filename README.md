# JaesolOS — Hobby Operating System Experiment

An experimental operating system project exploring low-level boot processes and kernel development.
The repository contains a partial 32-bit x86 boot demo, alongside plans to explore RISC-V,
FPGA-based systems, and networking.

**Status:** Experimental / Work in Progress  
**Technologies:** x86 Assembly, C, Multiboot, GRUB, RISC-V (planned), FPGA (planned)

![JaesolOS booting in QEMU](images/Screenshot%202026-04-20%20at%2012.30.46.png)

## What works today

The x86 demo builds from a clean checkout and boots in QEMU (checked on 2026-11-08, see [Verification](#verification)).
GRUB loads the kernel through Multiboot, and the kernel writes a boot log and a system summary to the
VGA text screen, then halts.

| Part | What it does |
| --- | --- |
| [`boot/boot.asm`](boot/boot.asm) | Multiboot v1 header (magic `0x1BADB002`; flags ask GRUB to page-align modules and to pass memory information), a 16 KiB stack, a call to `kernel_main`, and a `cli`/`hlt` loop if it returns. |
| [`boot/linker.ld`](boot/linker.ld) | ELF layout with entry `_start`, loaded at 1 MiB, Multiboot header first. |
| [`kernel/vga.c`](kernel/vga.c), [`include/vga.h`](include/vga.h) | VGA text-mode driver: writes to `0xB8000` (80×25), 16 colours, newline, carriage return, tab, and scrolling. |
| [`kernel/kernel.c`](kernel/kernel.c) | `kernel_main`: prints the JaesolOS banner, the boot log and the system summary. |
| [`iso/boot/grub/grub.cfg`](iso/boot/grub/grub.cfg) | GRUB menu entry `JaesolOS v0.1 - Phase 1` that runs `multiboot /boot/jaesol.bin`. |

Limits of the current demo:

- The boot-log lines (`[BIOS]`, `[GRUB]`, `[ASM ]`, `[ C  ]`) and the addresses in the summary are
  fixed strings in `kernel.c`; the kernel does not detect them at run time.
- The output is longer than 25 rows, so the banner at the top scrolls off the screen (as in the screenshot).
- There is no GDT/IDT setup, interrupt handling, keyboard input, shell, or memory management yet.
  The kernel's own message names the next step: *Phase 2 — GDT / IDT / Interrupts*.

## Build and run

Requirements: `nasm`, `gcc` able to emit 32-bit code (`-m32`; no 32-bit C library is needed because the
kernel is freestanding), GNU `ld`, and `grub-mkrescue` with BIOS GRUB modules and `xorriso`
(on Ubuntu: `nasm gcc grub-pc-bin xorriso`). QEMU for running it.

```sh
make            # builds iso/boot/jaesol.bin and JaesolOS.iso
qemu-system-i386 -cdrom JaesolOS.iso
make clean
```

## Verification

On 2026-11-08 a clean `git archive` export of `main` was built and booted with:
Ubuntu 24.04.4, gcc 13.3.0, GNU ld 2.42, NASM 2.16.01, GRUB 2.12 (`grub-pc-bin`), xorriso 1.5.6, QEMU 8.2.2.

- `make` completed; `iso/boot/jaesol.bin` is a 32-bit ELF (Intel 80386) with entry `_start` at `0x101000`,
  and `grub-file --is-x86-multiboot` accepts it.
- The ISO booted in `qemu-system-i386` (no network device) and showed the same screen as the screenshot,
  ending with `CPU halted. (boot.asm .halt loop)`.
- The kernel inside the previously committed `JaesolOS.iso` was byte-identical to the one built from
  source, so the ISO was removed from the repository; build it with `make`.

## Repository layout

| Path | Contents |
| --- | --- |
| [`boot/`](boot) | `boot.asm` (Multiboot header and entry), `linker.ld` (ELF layout) |
| [`kernel/`](kernel), [`include/`](include) | `kernel.c`, the VGA driver `vga.c` and its header `vga.h` |
| [`iso/boot/grub/`](iso/boot/grub) | `grub.cfg`; `make` adds `iso/boot/jaesol.bin` and builds `JaesolOS.iso` |
| [`images/`](images) | QEMU screenshot |
| [`docs/PROJECT_PLAN.md`](docs/PROJECT_PLAN.md) | RISC-V / FPGA plan |

Build outputs (`*.o`, `*.bin`, `*.iso`) are not tracked (see [`.gitignore`](.gitignore)).

## Planned RISC-V and FPGA work

This part is a plan only: no RV32 code or FPGA deployment exists or has been verified yet.
The [project plan](docs/PROJECT_PLAN.md) starts with an RV32 kernel that prints a boot banner under
QEMU, then adds networking (virtio-net and lwIP) and a small HTTP application, *JaesolCaddy*.
Later milestones run it on a RISC-V soft core in the FPGA fabric of a Terasic DE10-Standard board,
behind an existing Caddy reverse proxy.

## License

The repository does not currently include a license file.
