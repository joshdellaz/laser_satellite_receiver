#include <stdint.h>

// ignoring burst positions in LDPC for now

// #ifdef __cplusplus
// extern "C" {
// #endif
void initLDPC(void);
void applyLDPC(uint8_t * input);
void decodeLDPC(uint8_t * rxinput);
// #ifdef __cplusplus
// }
// #endif