typedef enum {
    LIQUID_CRC_UNKNOWN = 0,   // unknown/unavailable CRC scheme
    LIQUID_CRC_NONE,        // no error-detection
    LIQUID_CRC_CHECKSUM,    // 8-bit checksum
    LIQUID_CRC_8,           // 8-bit CRC
    LIQUID_CRC_16,          // 16-bit CRC
    LIQUID_CRC_24,          // 24-bit CRC
    LIQUID_CRC_32           // 32-bit CRC
} crc_scheme;

// generate error-detection key
//  _scheme     :   error-detection scheme
//  _msg        :   input data message, [size: _n x 1]
//  _n          :   input data message size
unsigned int crc_generate_key(crc_scheme      _scheme,
    unsigned char* _msg,
    unsigned int    _n);
