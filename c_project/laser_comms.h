//#include <cstdlib>
//#include <stdbool.h>
#include <stdint.h>
//#include "tavildarLDPC/LdpcC/LdpcCode.h"
//#include "packet_frame.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void applyLDPC(uint8_t *); // TODO: rename
    void decodeLDPC(uint8_t *);
#ifdef __cplusplus
}
#endif
