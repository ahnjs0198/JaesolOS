# Jaesol Stack — RISC-V Implementation Plan (v1.1)

**Last updated:** 2026-10-28  
**State:** Planning / project reactivated. No RV32 code or FPGA deployment has been verified.  
**Repo:** https://github.com/ahnjs0198/JaesolOS  
**Hardware:** Terasic DE10-Standard; Cyclone V SoC, 64 MiB SDRAM accessible from FPGA side, ARM HPS, on-board HPS-connected RJ45. Verify exact peripherals/pinout before implementation.

## Goal

Deliver a public demo HTML page generated on a **RISC-V soft-core running inside the FPGA fabric**, under **JaesolOS** and **JaesolCaddy**, with an existing Linux Caddy reverse proxy handling HTTPS. Keep production jaesolahn.com untouched. Start at a separate hostname such as `stack.jaesolahn.com` when approved.

Priorities: (1) feasibility, (2) fun & visible milestones, (3) fast AI-assisted iteration, (4) learning only when needed.

## Architecture contract

```text
                          HTTPS
browser <--------------------------------> existing Linux Caddy
                                             |
                                        internal HTTP
                                             |
                                   DE10-Standard network path
                                             |
                                          lwIP TCP/IP
                                             |
                                    JaesolCaddy (HTTP)
                                             |
                           JaesolOS RV32 kernel / event loop
                                             |
                                RV32 soft-core in FPGA fabric
```

### Hardware uncertainty: gate before committing to Ethernet approach

DE10-Standard's on-board RJ45 PHY is wired to the ARM HPS Ethernet MAC, **not** to a LiteEth fabric MAC. First check:

- Which Quartus Prime Lite version and Cyclone V SoC device support package successfully synthesize the exact device?
- Do LiteX/other maintained projects have a *DE10-Standard* platform/target, or is custom pin/clock/SDRAM integration needed?
- Can RV32 + UART run with FPGA RAM first, then FPGA-side 64 MiB SDRAM?
- Option A: HPS networking proxy/bridge feeding FPGA RISC-V via AXI; preserve clear separation of code executing on the FPGA vs HPS.
- Option B: a compatible external SPI Ethernet MAC+PHY connected to FPGA GPIO/HSMC, with lwIP TCP/IP on RV32.
- Temporary serial/network tunnel is allowed for intermediate demo only.
- Do not confuse DE10-Standard with DE10-Nano or DE10-Lite, or assume all three share pinouts/support.

**Decision artifact:** `docs/HARDWARE_FEASIBILITY.md` with tested toolchain, board pinout, memory boot, Ethernet route choice, evidence, fallback and cost. Do not buy hardware until an option is substantiated.

## Detailed milestone contracts

| Stage | Deliverable | Pass/fail evidence | Exclusions |
| --- | --- | --- | --- |
| P0 | read-only repository inventory, legacy x86 build check | build output + QEMU serial/VGA or diagnostic report | no legacy overwrite |
| P1 | minimal RV32 QEMU kernel | QEMU UART exact boot banner, CI/test script | paging, userspace |
| P2 | virtio-net + lwIP | ARP/ICMP packets, pcap and deterministic tests | full socket/POSIX API |
| P3 | JaesolCaddy v0.1 | `curl -i` HTTP 200 and dynamic counter from QEMU OS | Go/Caddy runtime |
| P4 | optional externally visible demo | remote HTTPS request traverses Caddy to QEMU origin | changing main website |
| P5 | DE10-Standard RV32 kernel boot | bitstream/console logs, soft-core evidence | ARM-only execution |
| P6 | FPGA-backed site | remote request + FPGA runtime counter, cold reboot reproducibility | fake static Linux response |

### Technical shape

- `arch/riscv32/qemu` vs `arch/riscv32/de10_standard`: startup, memory map, UART, trap, timers, NIC backend.
- `kernel`: minimal fixed allocator or static arena and single event loop.
- `net`: lwIP port; device-independent packet interface; no dependency on general processes.
- `apps/jaesolcaddy`: static HTML, GET / and status endpoint, uptime/request count.
- `tests`: boot banner, host pcap, HTTP e2e checks. Integrate on QEMU early.
- Maintain old x86 files unchanged in initial migration. Rearrangement only after successful P0 review.

## Recommended first Claude Code task

1. Inspect all files in the current repository; verify names/paths and whether prior README matches actual source.
2. Run or diagnose original x86 build and QEMU launch without discarding changes.
3. Establish reproducible toolchain/CI or local tests; **report what actually ran**.
4. Inspect DE10-Standard board files, Quartus Prime Lite device support and LiteX target availability. Explicitly investigate HPS-only on-board Ethernet.
5. Draft `docs/HARDWARE_FEASIBILITY.md` and a minimal RV32 QEMU `virt` boot plan.
6. Report blockers and next small action, obtain approval before changing network equipment or flashing FPGA.
7. Commit only scoped, tested work with clear evidence.

## Definition of done

An actual browser reaches a dynamically generated response from JaesolCaddy on a soft-core RV32 in DE10-Standard FPGA fabric. A Linux reverse proxy may provide TLS but cannot generate the origin page. Test by checking a counter/uptime, power-cycling FPGA, and observing origin availability. Public deployment and DNS changes require approval.

## Risks and fallbacks

- No ready DE10-Standard LiteX target → custom platform integration or different maintained SoC infrastructure; do not assume DE10-Lite build will work.
- On-board Ethernet is HPS-connected → explicit HPS bridge or externally connected FPGA Ethernet hardware; label which was used.
- FPGA RAM / SDRAM controller issues → first boot from embedded RAM to prove RV32, then add SDRAM.
- QEMU `virtio-net` vs physical NIC mismatch → abstract packet I/O from TCP/IP.
- TLS and internet attacks → existing Linux Caddy, isolated backend, no direct FPGA public exposure.
- Stalled milestone → freeze a demo and document genuine evidence rather than reporting plans as completed.

## Related resources

- https://www.marutsu.co.jp/contents/shop/marutsu/datasheet/terasic_DE10-Standard.pdf
- https://www.intel.com/content/www/us/en/products/details/fpga/development-tools/quartus-prime/resource.html
- https://github.com/litex-hub/litex-boards
- https://github.com/enjoy-digital/litex
- https://www.qemu.org/docs/master/system/riscv/virt.html
- https://savannah.nongnu.org/projects/lwip/
