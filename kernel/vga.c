/*
 * vga.c  —  VGA 텍스트 모드 드라이버 구현
 *
 * 이 파일이 하는 일:
 *   커널이 화면에 글자를 출력할 수 있도록 낮은 레벨의 VGA 하드웨어를 제어.
 *   printf 같은 건 없으니 이게 우리의 "화면 출력 시스템" 전부.
 *
 * Memory-Mapped I/O 개념:
 *   일반적으로 하드웨어에 명령을 내리려면 특별한 I/O 포트 명령이 필요.
 *   하지만 VGA 텍스트 버퍼는 예외적으로 메모리 주소에 직접 매핑되어 있음.
 *   즉, uint16_t *ptr = (uint16_t*)0xB8000 하고 *ptr = 값 하면
 *   화면 왼쪽 위에 글자가 나타남.
 */
#include "vga.h"

/* VGA 텍스트 버퍼 상수 */
#define VGA_COLS    80          /* 가로 80칸 */
#define VGA_ROWS    25          /* 세로 25줄 */
#define VGA_BUF     ((uint16_t*)0xB8000)   /* 물리 메모리 주소 직접 캐스팅 */

/*
 * 색상 속성 1바이트 만들기
 * 상위 4비트 = 배경색, 하위 4비트 = 전경색
 * 예) fg=WHITE(15=0xF), bg=BLACK(0) → 0x0F
 */
static inline uint8_t make_color(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)(fg | (bg << 4));
}

/*
 * VGA 엔트리 2바이트 만들기
 * 상위 1바이트 = 색상 속성, 하위 1바이트 = ASCII 코드
 * 예) 'A'(0x41), color=0x0F → 0x0F41
 */
static inline uint16_t make_entry(char c, uint8_t color) {
    return (uint16_t)((uint16_t)color << 8 | (uint8_t)c);
}

/* 드라이버 내부 상태 */
static size_t   cur_row;        /* 현재 커서 행 (0~24) */
static size_t   cur_col;        /* 현재 커서 열 (0~79) */
static uint8_t  cur_color;      /* 현재 출력 색상 */
static uint16_t *buf;           /* VGA 버퍼 포인터 */

void vga_init(void) {
    cur_row   = 0;
    cur_col   = 0;
    cur_color = make_color(VGA_LIGHT_GREY, VGA_BLACK);
    buf       = VGA_BUF;
    vga_clear();
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    cur_color = make_color(fg, bg);
}

void vga_clear(void) {
    uint16_t blank = make_entry(' ', cur_color);
    for (size_t i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        buf[i] = blank;     /* 2000개 셀 전부 공백으로 채움 */
    }
    cur_row = 0;
    cur_col = 0;
}

/*
 * 스크롤: 화면이 꽉 차면 모든 줄을 한 줄씩 위로 올리고
 * 맨 아래 줄을 공백으로 채움.
 * 마치 터미널 창에서 엔터를 계속 치는 것과 같은 효과.
 */
static void scroll(void) {
    /* 1~24행 내용을 0~23행으로 복사 */
    for (size_t r = 1; r < VGA_ROWS; r++)
        for (size_t c = 0; c < VGA_COLS; c++)
            buf[(r-1)*VGA_COLS + c] = buf[r*VGA_COLS + c];

    /* 마지막(24행)을 공백으로 */
    uint16_t blank = make_entry(' ', cur_color);
    for (size_t c = 0; c < VGA_COLS; c++)
        buf[(VGA_ROWS-1)*VGA_COLS + c] = blank;

    cur_row = VGA_ROWS - 1;
}

void vga_putchar(char c) {
    if (c == '\n') {                    /* 줄바꿈 */
        cur_col = 0;
        cur_row++;
    } else if (c == '\r') {             /* 캐리지 리턴 */
        cur_col = 0;
    } else if (c == '\t') {             /* 탭: 8칸 단위 경계로 이동 */
        cur_col = (cur_col + 8) & ~(size_t)7;
    } else {
        /* 일반 문자: VGA 버퍼에 직접 기록 */
        buf[cur_row * VGA_COLS + cur_col] = make_entry(c, cur_color);
        cur_col++;
    }

    if (cur_col >= VGA_COLS) {          /* 줄 끝 도달 → 다음 줄 */
        cur_col = 0;
        cur_row++;
    }
    if (cur_row >= VGA_ROWS) {          /* 화면 끝 도달 → 스크롤 */
        scroll();
    }
}

void vga_puts(const char *s) {
    while (*s) vga_putchar(*s++);
}
