#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "crc.h"


static CcittCrc ccittcrc;
static Crc32 crc32;
static SdCardCrc16 sdcrc;
static SdCardCrc7 sdcrc7;

void expect(uint32_t got, uint32_t expected) {
    if (got != expected) 
        printf("Test Failed: got 0x%x, expected 0x%x\n", got, expected);
}


void test(const char *msg, size_t msglen, uint16_t e) {
    ccittcrc.init();
    ccittcrc.add((const uint8_t*)msg, msglen);
    expect(ccittcrc.end(), e);
}

void test32(const char *msg, size_t msglen, uint32_t e) {
    crc32.init();
    crc32.add((const uint8_t*)msg, msglen);
    expect(crc32.end(), e);
}

int main() {
    // See https://srecord.sourceforge.net/crc16-ccitt.html
    test("", 0, 0x1d0f);
    test("A", 1, 0x9479);
    test("123456789", 9, 0xe5cc);

    char a[256];
    memset(a, 'A', sizeof a);
    test(a, 256, 0xe938);

    test32("123456789", 9, 0xcbf43926);

    const uint8_t sd[] = { 0x01,0x02,0x03,0x04,0x05,0x01,0x02,0x03,0x04,0x05,0x01,0x02,0x03,0x04,0x05,0x06 };
    sdcrc.init();
    sdcrc.add(sd, sizeof sd);
    expect(sdcrc.end(), 0xe713);

    sdcrc.init();
    sdcrc.add((const uint8_t*)"123456789", 9);
    expect(sdcrc.end(), 0x31c3);

    sdcrc7.init();
    sdcrc7.add((const uint8_t*)"123456789", 9);
    expect(sdcrc7.end(), 0x75);
}
