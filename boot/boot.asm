; =============================================================
;  boot.asm  —  JaesolOS의 첫 번째 코드
;
;  역할 두 가지:
;    1) Multiboot 헤더 선언  → GRUB이 "이게 OS 커널이다!" 인식
;    2) 스택 설정 후 C 커널로 점프
;
;  왜 C가 아닌 어셈블리냐?
;    → C 코드 실행 전에 스택(ESP 레지스터)을 먼저 잡아야 하는데,
;      스택 설정 자체는 어셈블리로만 가능하기 때문.
; =============================================================

; ─── Multiboot 상수 ───────────────────────────────────────────
; GRUB은 커널 파일의 처음 8KB 안에서 아래 세 값을 4바이트 정렬로 찾는다.
; 세 값의 합이 반드시 0이 되어야 유효한 Multiboot 헤더로 인정.

MBOOT_MAGIC    equ 0x1BADB002   ; 고정된 매직 넘버 (GRUB 스펙에 정의됨)
MBOOT_FLAGS    equ 0x00000003   ; 비트0=4KB 정렬, 비트1=메모리 맵 제공
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)  ; 합이 0이 되게 하는 값

; .multiboot 섹션: 링커 스크립트에서 파일 맨 앞에 배치하도록 지시
section .multiboot
align 4
    dd MBOOT_MAGIC      ; dd = 4바이트(32bit) 값을 직접 기록
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

; ─── 커널 스택 공간 예약 (16 KB) ─────────────────────────────
; section .bss = 초기화 안 된 데이터. 실제 파일 크기를 차지하지 않음.
; resb N = N바이트 공간 예약 (Reserve Byte)
section .bss
align 16
stack_bottom:
    resb 16384      ; 16 * 1024 = 16KB. 위쪽이 stack_top
stack_top:
; 스택은 위에서 아래로 자란다. ESP = stack_top 으로 시작.

; 스택 실행 불필요 표시 (링커 경고 제거용)
section .note.GNU-stack noalloc noexec nowrite progbits

; ─── 실제 실행 코드 ──────────────────────────────────────────
section .text
global _start       ; _start 를 외부에서 참조 가능하게 공개
extern kernel_main  ; kernel.c 에서 정의될 C 함수

_start:
    ; ① 스택 포인터(ESP)를 우리가 예약한 공간 꼭대기로 설정
    ;    이 한 줄이 없으면 C 코드에서 함수 호출 즉시 메모리 오염 발생
    mov esp, stack_top

    ; ② C 커널 메인 함수 호출
    ;    call 명령은 리턴 주소를 스택에 push하고 점프함
    call kernel_main

    ; ③ kernel_main이 혹시 return 하면 여기서 영구 정지
    ;    정상적으로는 여기 도달하면 안 됨
.halt:
    cli         ; Clear Interrupt Flag — 인터럽트를 꺼서 깨어나지 않게
    hlt         ; Halt — CPU 완전 정지
    jmp .halt   ; 만약 NMI 같은 예외로 깨어나도 다시 정지
