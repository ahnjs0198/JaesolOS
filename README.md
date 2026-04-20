# JaesolOS

A hobby x86 operating system built from scratch for learning purposes.

## Current Status

**Phase 1 — Bootloader & VGA Driver** ✅
- Multiboot-compliant kernel (GRUB compatible)
- 32-bit Protected Mode entry via assembly
- VGA text mode driver (80×25, 16 colors)
- Kernel boots and prints system info to screen

## Project Structure

```
JaesolOS/
├── boot/
│   ├── boot.asm      # Assembly entry point + Multiboot header
│   └── linker.ld     # Linker script (kernel loads at 1MB)
├── kernel/
│   ├── kernel.c      # kernel_main() — C entry point
│   └── vga.c         # VGA text mode driver
├── include/
│   └── vga.h         # VGA driver interface
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg  # GRUB boot menu config
└── Makefile
```

## How It Works

```
Power ON
  └─> BIOS/UEFI    hardware init
        └─> GRUB   finds Multiboot header, loads kernel ELF at 0x100000
              └─> boot.asm  sets up 16KB stack, calls kernel_main()
                    └─> kernel.c  VGA driver init, prints boot log
```

## Build & Run

### Requirements

```bash
# Ubuntu / Debian
sudo apt install build-essential nasm qemu-system-x86 \
                 grub-pc-bin grub-common xorriso mtools

# macOS
brew install nasm qemu xorriso
# (grub requires additional setup on macOS — use a Linux VM or Docker)
```

### Build

```bash
make        # builds JaesolOS.iso
make clean  # removes build artifacts
```

### Run

```bash
qemu-system-i386 -cdrom JaesolOS.iso
```

Or burn the ISO to a USB drive and boot on real hardware.

## Roadmap

- [x] Phase 1 — Bootloader, Protected Mode, VGA driver
- [ ] Phase 2 — GDT, IDT, interrupt handling, keyboard input
- [ ] Phase 3 — Physical memory manager, paging
- [ ] Phase 4 — Process scheduler
- [ ] Phase 5 — Virtual filesystem, syscalls, user mode shell

## References

- [OSDev Wiki](https://wiki.osdev.org)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [Writing a Simple OS from Scratch — Nick Blundell](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
