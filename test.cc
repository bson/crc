#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "crc.h"


static CcittCrc crc16;
static Crc32 crc32;

void test(const char *msg, size_t msglen, uint16_t expect) {
    crc16.init();
    crc16.add((const uint8_t*)msg, msglen);
    printf("0x%04x  0x%04x\n", expect, crc16.end());
    //assert(ccitt.end() == expect);
}

void test32(const char *msg, size_t msglen, uint32_t expect) {
    crc32.init();
    crc32.add((const uint8_t*)msg, msglen);
    printf("0x%08x  0x%08x\n", expect, crc32.end());
    //assert(crc32.end() == expect);
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
}
