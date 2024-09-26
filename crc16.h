#include <stdint.h>
#include <stddef.h>


// Non-bit reversed CRC-16.  The reason for this preference over bit
// reversed is that 16-bit polynomials are typically not specified in
// a reverse bit order (unlike CRC-32).

class Crc16 {
    uint16_t _crcval[256];         // Precomputed CRCs for byte values
    uint16_t _crc;

public:
    enum class Poly : uint16_t { 
        // These are from https://en.wikipedia.org/wiki/Cyclic_redundancy_check
        CCITT     = 0x1021,     // CCITT: SD Cards, etc
        CHAK      = 0x2f15,     // Chakravarty
        ARINC     = 0xa02b,     // ARINC
        CDMA2000  = 0xc867,     // 
        DECT      = 0x0589,     // 
        SCSI_DIFF = 0x8bb7,     // Diff. SCSI
        DNP       = 0x3d65,     // M-Bus
        IBM       = 0x8005,     // 
    };

    enum class Init : uint16_t {
        CCITT     = 0x1d0f,     // CCITT, non-bit reversed
    };

    Crc16() = default;
    ~Crc16() = default;

    // Call this first to construct table
    void init(Poly poly) {
        for (uint16_t b = 0; b < 256; b++) {
            uint16_t crc = b << 8;
            for (uint8_t i = 0; i < 8; i++) {
                if (crc & 0x8000)
                    crc = (crc << 1) ^ uint16_t(poly);
                else
                    crc <<= 1;
            }
            _crcval[b] = crc;
        }
    }

    // Start new check, returns CRC
    uint16_t start(Init initval) {
        _crc = uint16_t(initval);
        return _crc;
    }

    // Add byte, returns CRC
    uint16_t add(uint8_t val) {
        _crc = _crcval[uint8_t((_crc >> 8) ^ val)] ^ (_crc << 8);
        return _crc;
    }

    // Byte sequence, returns CRC
    uint16_t add(const uint8_t* val, size_t len) {
        if (len == 0)
            return _crc;

        while (len-- > 1)
            add(*val++);

        return add(*val);
    }

    // End check, returns CRC.  Note that sometimes the one's
    // complement is used (which does nothing beyond spreading
    // confusion).
    uint16_t end() const {
        return _crc;
    }
};

