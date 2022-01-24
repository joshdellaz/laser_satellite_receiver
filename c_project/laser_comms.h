//#include <cstdint>
//#include <stdbool.h>

#ifdef __cplusplus
extern "C"
#endif
void applyLDPC();

#define CODEWRD_L 1296
#define CODEWRD_R 0.5
#define MAX_DECODE_ITERS 25
#define MIN_SUM 1