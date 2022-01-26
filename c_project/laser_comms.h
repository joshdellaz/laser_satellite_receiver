//#include <cstdlib>
//#include <stdbool.h>
#include <stdint.h>
//#include "packet_frame.h"

#ifdef __cplusplus
extern "C" {
#endif
void applyLDPC(uint8_t *);
void decodeLDPC(uint8_t *);
#ifdef __cplusplus
}
#endif

#define CODEWRD_L 1296
#define CODEWRD_R 0.5
#define MAX_DECODE_ITERS 25
#define MIN_SUM 1
