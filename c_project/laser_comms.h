#include <stdint.h>

// ignoring burst positions in LDPC for now

#ifdef __cplusplus
extern "C" {
#endif
void applyLDPC(uint8_t *);
void decodeLDPC(uint8_t *);
#ifdef __cplusplus
}
#endif