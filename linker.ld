/*
 * linker.ld  —  ELF 출력용으로 수정
 *
 * 왜 이전 버전에서 에러가 났나?
 *   --oformat binary 옵션으로 raw 바이너리를 만들었는데,
 *   GRUB의 'multiboot' 명령은 ELF 포맷 파일을 파싱해서 로드함.
 *   ELF 헤더가 없으니 "invalid arch-dependent ELF magic" 에러 발생.
 *
 * 해결:
 *   --oformat binary 제거 → 기본 ELF 포맷으로 출력.
 *   GRUB이 ELF 헤더를 읽고 각 섹션을 올바른 주소에 로드함.
 */

ENTRY(_start)

SECTIONS {
    /*
     * 커널 로드 주소: 1MB (0x00100000)
     * GRUB Multiboot 스펙에서 커널은 1MB 이상에 로드되어야 함.
     * 0~1MB는 BIOS, VGA, GRUB 등이 사용하는 영역.
     */
    . = 0x00100000;

    /* Multiboot 헤더를 파일 앞쪽에 배치 (GRUB이 첫 8KB 안에서 탐색) */
    .multiboot ALIGN(4) : {
        *(.multiboot)
    }

    /* 실행 코드 */
    .text ALIGN(4K) : {
        *(.text)
    }

    /* 읽기 전용 데이터 (문자열 리터럴 등) */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    /* 초기화된 전역 변수 */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* 초기화되지 않은 변수 + 스택 */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }
}
