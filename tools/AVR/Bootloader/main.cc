#include <stdint.h>
#include <avr/boot.h>

uint8_t array[256]={'A','B'};

int main(void) {
    uint8_t *buf=array;
    uint32_t page=0;
    uint16_t w;
    uint8_t y;
    uint16_t i;
    for (y=0;y<100;++y) {
        page=((uint16_t)y)<<8;
        for (i=0; i<10; i+=2) {
            w = (buf[i+1]);
            w<<=8;
            w|= buf[i];
            __asm__ __volatile__
            (
                "movw  r0, %4\n\t"
                "movw r30, %A3\n\t"
                "sts %1, %C3\n\t"
                "sts %0, %2\n\t"
                "spm\n\t"
                "clr  r1\n\t"
              :
              :
                  "i" (_SFR_MEM_ADDR(__SPM_REG)),
                  "i" (_SFR_MEM_ADDR(RAMPZ)),
                  "r" ((uint8_t)__BOOT_PAGE_FILL),
                  "r" ((uint32_t)(page+i)),
                  "r" ((uint16_t)w)
                : "r0", "r1", "r30", "r31"
                );
        }
    }
    return 0;
}
