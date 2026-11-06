# JaesolOS

JaesolOS is a hobby operating-system experiment. The repository contains a partial 32-bit x86 boot demo and a proposal for a future RISC-V/FPGA system.

## x86 boot demo

| Area | Evidence in the repository |
| --- | --- |
| Multiboot and entry point | [boot.asm](boot.asm) defines a Multiboot v1 header, sets up a 16 KiB stack, calls `kernel_main`, and halts when it returns. |
| ELF layout | [linker.ld](linker.ld) declares `_start`, places the image at 1 MiB, and orders the Multiboot section first. |
| VGA banner | [kernel.c](kernel.c) calls a VGA text API to print a fixed banner and status messages. The driver and its header are absent from the tracked source. The messages are strings embedded in the kernel. |
| Build and GRUB | The [Makefile](Makefile) describes a 32-bit ELF build and calls `grub-mkrescue`. Several paths it expects are absent, including the VGA implementation, its header, and the GRUB configuration. The checkout cannot reproduce the ISO as written. |
| Historical artifact | A previously built ISO is tracked. Its provenance and runtime behavior have not been independently verified. |

The demo code has no interactive shell or keyboard input.

## Planned RISC-V and FPGA work

The proposed path starts with an RV32 boot banner under QEMU, then adds networking and a small HTTP application. Running the application on an FPGA soft core and serving a request through a reverse proxy are later milestones. See the [project plan](docs/PROJECT_PLAN.md).

## License

The repository does not currently include a license file.
