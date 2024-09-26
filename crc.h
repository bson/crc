#include <stdint.h>
#include <stddef.h>

// Common polynomials
enum CrcPoly { 
    // These are from https://en.wikipedia.org/wiki/Cyclic_redundancy_check

    // Polynomials
    // These are from https://en.wikipedia.org/wiki/Cyclic_redundancy_check
    CCITT     = 0x1021,     // CCITT: SD Cards, etc
    CHAK      = 0x2f15,     // Chakravarty
    ARINC     = 0xa02b,     // ARINC
    CDMA2000  = 0xc867,     // 
    DECT      = 0x0589,     // 
    SCSI_DIFF = 0x8bb7,     // Diff. SCSI
    DNP       = 0x3d65,     // M-Bus
    IBM       = 0x8005,     // 

    MMC       = 0x09,       // MMC (CRC-7)

    CRC32     = 0xedb88320,    // Bit reversed 0x04c11db7
                               // ISO 3309 (HDLC), ANSI X3.66, V.42,
                               // Ethernet, SATA, MPEG-2,
                               // PKZIP, GZIP, BZIP2, POSIX cksum, PNG, ZMODEM, etc.
    SCSI      = 0x82f63b80,    // SCSI; 0x1dc6f41) 
    KOOPMAN   = 0xeb31d82e,    // Excellent for short messages; 0x741b8cd7
    KOOPMAN2  = 0x992c1a4c,    // 0x32583499
};


// General CRC
// Value is underlying type, e.g. uint16_t or uint32_t for CRC-16 and CRC-32 respectively.
// POLY is the polynomial.
// REVERSE means the table is reversed.
// REFLECTED means bytes are added in reverse bit endianness
// XOR_RESULT is used to invert the result.
// INIT is the initialization value.

template <
    typename Value, Value POLY,
    bool REVERSED, bool REFLECTED, Value XOR_RESULT, Value INIT,
    size_t WIDTH = sizeof(Value)*8
>
class Crc {
    Value _crcval[256];         // Precomputed CRCs for byte values
    Value _crc;

public:
    Crc() = default;
    ~Crc() = default;

    enum { MSB = (Value(~0ULL) >> 1) + 1 };
    enum { SHIFT = sizeof (Value) * 8 - WIDTH }; // Align to MSB

    // Call this first to construct table
    void init() {
        for (int b = 0; b < 256; b++) {
            Value crc;
            if (REVERSED) {
                crc = b;
                for (uint8_t i = 0; i < 8; i++)
                    crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            } else {
                crc = WIDTH <= 8 ? b : b << (WIDTH-8);
                for (uint8_t i = 0; i < 8; i++)
                    crc = crc & MSB ? (crc << 1) ^ (POLY << SHIFT) : crc << 1;
            }
            _crcval[b] = crc;
        }
        _crc = INIT << SHIFT;
    }

    // Reset without rebuilding table
    void reset() { _crc = INIT << SHIFT; }

    // Add byte
    void add(uint8_t val) {
        if (WIDTH <= 8)
            _crc = _crcval[_crc ^ val];
        else if (REFLECTED)
            _crc = _crcval[uint8_t(_crc) ^ val] ^ (_crc >> 8);
        else
            _crc = _crcval[uint8_t((_crc >> 8) ^ val)] ^ (_crc << 8);
    }

    // Add byte sequence
    void add(const uint8_t* val, size_t len) {
        while (len-- > 0)
            add(*val++);
    }

    Value end() const { 
        return (_crc >> SHIFT) ^ XOR_RESULT;
    }
};


typedef Crc<uint16_t, CrcPoly::CCITT, false, false, 0, 0x1d0f> CcittCrc;
typedef Crc<uint16_t, CrcPoly::CCITT, false, false, 0xffff, 0xffff> X25Crc;
typedef Crc<uint32_t, CrcPoly::CRC32, true, true, 0xffffffff, 0xffffffff> Crc32;
typedef Crc<uint16_t, CrcPoly::CCITT, false, false, 0, 0> SdCardCrc16;
typedef Crc<uint8_t, CrcPoly::MMC, false, false, 0, 0, 7> SdCardCrc7;
