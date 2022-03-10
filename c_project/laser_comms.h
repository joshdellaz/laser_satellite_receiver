//#include <cstdlib>
//#include <stdbool.h>
#include <stdint.h>
//#include "tavildarLDPC/LdpcC/LdpcCode.h"
//#include "packet_frame.h"

#ifdef __cplusplus
extern "C"
{
#endif
    // Tavildar code with WIFI LDPC matrices
    void applyLDPC_WIFI(uint8_t *); 
    void decodeLDPC_WIFI(uint8_t *);
    
    // 5G low-rate LDPC
    void encode_LDPC(uint8_t *);
    void decode_LDPC(uint8_t *);
    
#ifdef __cplusplus
}
#endif
