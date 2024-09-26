#include <stdint.h>
#include <stddef.h>

// Bit reversed CRC-32.  The reason for this preference over non-bit
// reversed is that 32-bit polynomials are typically specified in
// reverse bit order (unlike CRC-16).
class Crc32 {
    uint32_t _crcval[256];         // Precomputed CRCs for byte values
    uint32_t _crc;

public:
    enum class Poly : uint32_t { 
        // These are from https://en.wikipedia.org/wiki/Cyclic_redundancy_check

        // Polynomials
        COMMON    = 0xedb88320,    // Bit reversed 0x04c11db7
                                   // ISO 3309 (HDLC), ANSI X3.66, V.42,
                                   // Ethernet, SATA, MPEG-2,
                                   // PKZIP, GZIP, BZIP2, POSIX cksum, PNG, ZMODEM, etc.
        SCSI      = 0x1dc6f41,     // SCSI
        KOOPMAN   = 0x741b8cd7,    // Excellent for short messages
        KOOPMAN2  = 0x32583499,
    };

    enum class Init : uint32_t {
        CRC32     = 0xffffffff,
    };

    Crc32() = default;
    ~Crc32() = default;

    // Call this first to construct table
    void init(Poly poly) {
        for (uint32_t b = 0; b < 256; b++) {
            uint32_t crc = b;
            for (uint8_t i = 0; i < 8; i++) {
                if (crc & 1)
                    crc = (crc >> 1) ^ uint32_t(poly);
                else
                    crc >>= 1;
            }
            _crcval[b] = crc;
        }
    }

    // Start new check, returns CRC
    uint32_t start(Init initval) {
        _crc = uint32_t(initval);
        return _crc;
    }

    // Add byte, reverse order, returns CRC.
    uint32_t add(uint8_t val) {
        _crc = _crcval[uint8_t(_crc) ^ val] ^ (_crc >> 8);
        return _crc;
    }

    // Byte sequence, returns CRC
    uint32_t add(const uint8_t* val, size_t len) {
        if (len == 0)
            return _crc;

        while (len-- > 1)
            add(*val++);

        return add(*val);
    }

    // End check, returns CRC.  Note that sometimes the one's
    // complement is used (which does nothing beyond spreading
    // confusion).
    uint32_t end() const {
        return ~_crc;
    }
};
