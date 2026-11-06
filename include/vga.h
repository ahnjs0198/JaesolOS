/*
 * vga.h  —  VGA 텍스트 모드 드라이버 인터페이스
 *
 * VGA 텍스트 모드란?
 *   PC가 부팅하면 기본적으로 80열 × 25행 텍스트 화면 모드로 시작.
 *   GRUB도, 우리 커널도 초기엔 이 모드에서 동작.
 *
 *   핵심 아이디어:
 *   물리 메모리 0xB8000 번지가 VGA 하드웨어와 연결되어 있어서,
 *   이 주소에 값을 쓰면 즉시 화면에 반영됨 (Memory-Mapped I/O).
 *
 *   각 문자는 2바이트로 구성:
 *     [7:4] 배경색 (4비트)
 *     [3:0] 전경색 (4비트)
 *     [7:0] ASCII 문자 (1바이트)
 *   → 합쳐서 16비트(uint16_t) 1개
 */
#ifndef VGA_H
#define VGA_H

#include <stdint.h>   /* uint8_t, uint16_t, uint32_t */
#include <stddef.h>   /* size_t */

/* VGA 16색 팔레트 */
typedef enum {
    VGA_BLACK         = 0,  VGA_BLUE        = 1,
    VGA_GREEN         = 2,  VGA_CYAN        = 3,
    VGA_RED           = 4,  VGA_MAGENTA     = 5,
    VGA_BROWN         = 6,  VGA_LIGHT_GREY  = 7,
    VGA_DARK_GREY     = 8,  VGA_LIGHT_BLUE  = 9,
    VGA_LIGHT_GREEN   = 10, VGA_LIGHT_CYAN  = 11,
    VGA_LIGHT_RED     = 12, VGA_PINK        = 13,
    VGA_YELLOW        = 14, VGA_WHITE       = 15,
} vga_color_t;

void vga_init(void);                        /* 드라이버 초기화, 화면 지우기 */
void vga_set_color(vga_color_t fg,
                   vga_color_t bg);         /* 이후 출력할 글자 색 설정 */
void vga_putchar(char c);                   /* 문자 1개 출력 */
void vga_puts(const char *str);             /* 문자열 출력 */
void vga_clear(void);                       /* 화면 전체 지우기 */

#endif
