#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "crc16.h"
#include "crc32.h"


static Crc16 ccitt;
static Crc32 crc32;

void test(Crc16::Poly poly, Crc16::Init init, const char *msg, size_t msglen, uint16_t expect) {
    ccitt.init(poly);
    ccitt.start(init);
    ccitt.add((const uint8_t*)msg, msglen);
    //printf("0x%04x  0x%04x\n", expect, ccitt.end());
    assert(ccitt.end() == expect);
}

void test32(Crc32::Poly poly, Crc32::Init init, const char *msg, size_t msglen, uint32_t expect) {
    crc32.init(poly);
    crc32.start(init);
    crc32.add((const uint8_t*)msg, msglen);
    //printf("0x%08x  0x%08x\n", expect, crc32.end());
    assert(crc32.end() == expect);
}

int main() {
    // See https://srecord.sourceforge.net/crc16-ccitt.html
    test(Crc16::Poly::CCITT, Crc16::Init::CCITT, "", 0, 0x1d0f);
    test(Crc16::Poly::CCITT, Crc16::Init::CCITT, "A", 1, 0x9479);
    test(Crc16::Poly::CCITT, Crc16::Init::CCITT, "123456789", 9, 0xe5cc);

    char a[256];
    memset(a, 'A', sizeof a);
    test(Crc16::Poly::CCITT, Crc16::Init::CCITT, a, 256, 0xe938);

    test32(Crc32::Poly::COMMON, Crc32::Init::CRC32, "123456789", 9, 0xcbf43926);
}
