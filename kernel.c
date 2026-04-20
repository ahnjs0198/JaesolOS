/*
 * kernel.c  —  JaesolOS kernel main
 *
 * NOTE: VGA text mode only supports ASCII + CP437 encoding.
 *       Korean (UTF-8/Unicode) characters cannot be displayed.
 *       All messages must be in English/ASCII.
 */
#include "vga.h"

static void print_hex(uint32_t n) {
    char hex[11];
    hex[0] = '0'; hex[1] = 'x';
    for (int i = 9; i >= 2; i--) {
        int d = n & 0xF;
        hex[i] = (d < 10) ? '0' + d : 'A' + d - 10;
        n >>= 4;
    }
    hex[10] = '\0';
    vga_puts(hex);
}

static void divider(void) {
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  ================================================\n");
}

void kernel_main(void) {

    vga_init();

    /* ── Title ── */
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("\n");
    vga_puts("        _____             _  ____   _____ \n");
    vga_puts("       |_   _|           | |/ __ \\ / ____|\n");
    vga_puts("         | |  __ _  ___  | | |  | | (___  \n");
    vga_puts("         | | / _` |/ _ \\ | | |  | |\\___ \\ \n");
    vga_puts("        _| || (_| |  __/ | | |__| |____) |\n");
    vga_puts("       |_____\\__,_|\\___| |_|\\____/|_____/ \n");

    vga_set_color(VGA_YELLOW, VGA_BLACK);
    vga_puts("\n              JaesolOS  v0.1  --  Phase 1\n");
    divider();

    /* ── Boot log ── */
    vga_puts("\n");

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  [BIOS]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("POST complete, hardware initialized\n");

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  [BIOS]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("Boot device search -> ISO found\n");

    vga_set_color(VGA_LIGHT_BLUE, VGA_BLACK);
    vga_puts("  [GRUB]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("Multiboot header signature verified (0x1BADB002)\n");

    vga_set_color(VGA_LIGHT_BLUE, VGA_BLACK);
    vga_puts("  [GRUB]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("Kernel loaded at physical address ");
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    print_hex(0x100000);
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("\n");

    vga_set_color(VGA_LIGHT_BLUE, VGA_BLACK);
    vga_puts("  [GRUB]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("Protected mode (32-bit) enabled\n");

    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts("  [ASM ]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("_start reached, kernel stack (16KB) set up\n");

    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts("  [ASM ]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("ESP = stack_top -> call kernel_main\n");

    vga_set_color(VGA_PINK, VGA_BLACK);
    vga_puts("  [ C  ]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("kernel_main() reached -- C runtime is working!\n");

    vga_set_color(VGA_PINK, VGA_BLACK);
    vga_puts("  [ C  ]  ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("VGA driver initialized at ");
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    print_hex(0xB8000);
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("\n");

    /* ── System info ── */
    vga_puts("\n");
    divider();
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("  System Info\n");
    divider();

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  Arch       : ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("x86 (IA-32), 32-bit Protected Mode\n");

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  Kernel at  : ");
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    print_hex(0x100000);
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts(" (1MB)\n");

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  VGA buffer : ");
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    print_hex(0xB8000);
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts(" (80x25 text mode)\n");

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  Stack size : ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("16 KB\n");

    /* ── Done ── */
    vga_puts("\n");
    divider();
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    vga_puts("  Phase 1 complete!\n");
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts("  Next: Phase 2 -- GDT / IDT / Interrupts\n");
    divider();
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    vga_puts("  CPU halted. (boot.asm .halt loop)\n\n");
}
