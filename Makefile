# =============================================================
#  Makefile  —  JaesolOS 빌드 자동화
#
#  make       : 전체 빌드 (ISO 생성)
#  make clean : 빌드 결과물 삭제
# =============================================================

CC     = gcc
AS     = nasm
LD     = ld

# 컴파일 옵션 설명:
#   -m32            32비트 x86 코드 생성 (64비트 머신에서도 32비트로)
#   -fno-pic        위치 독립 코드 비활성화 (커널은 고정 주소에 로드됨)
#   -ffreestanding  OS/표준 라이브러리 없는 환경임을 명시
#   -fno-builtin    컴파일러 내장 함수 최적화 비활성화
#   -nostdlib       표준 라이브러리 링크 안 함
#   -O2             최적화 레벨 2
CFLAGS = -m32 -fno-pic -ffreestanding -fno-builtin -nostdlib \
         -Wall -Wextra -O2 -I./include

# NASM 옵션: -f elf32 = 32비트 ELF 오브젝트 파일로 출력
ASFLAGS = -f elf32

# 링커 옵션:
#   -m elf_i386        32비트 ELF 형식으로 링크
#   -T boot/linker.ld  우리가 만든 링커 스크립트 사용
#   --oformat binary   순수 바이너리로 출력 (ELF 헤더 없는 raw 이미지)
# --oformat binary 를 제거해야 함!
# GRUB의 multiboot 명령은 ELF 포맷 커널을 요구함.
# raw binary를 주면 "invalid arch-dependent ELF magic" 에러 발생.
LDFLAGS = -m elf_i386 -T boot/linker.ld

OBJ    = boot/boot.o kernel/vga.o kernel/kernel.o
KERNEL = iso/boot/jaesol.bin
ISO    = JaesolOS.iso

.PHONY: all clean

all: $(ISO)

boot/boot.o: boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

kernel/vga.o: kernel/vga.c include/vga.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/kernel.o: kernel/kernel.c include/vga.h
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^
	@echo ""
	@echo "  ✅ 커널 빌드 성공: $(KERNEL)"
	@ls -lh $(KERNEL)

$(ISO): $(KERNEL)
	@grub-mkrescue -o $(ISO) iso/ 2>/dev/null
	@echo "  ✅ ISO 생성 성공: $(ISO)"
	@ls -lh $(ISO)
	@echo ""

clean:
	@rm -f $(OBJ) $(KERNEL) $(ISO)
	@echo "  🧹 빌드 파일 삭제 완료"
