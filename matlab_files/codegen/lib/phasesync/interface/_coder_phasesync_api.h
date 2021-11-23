/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_phasesync_api.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 15-Nov-2021 17:51:29
 */

#ifndef _CODER_PHASESYNC_API_H
#define _CODER_PHASESYNC_API_H

/* Include Files */
#include "emlrt.h"
#include "tmwtypes.h"
#include <string.h>

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void phasesync(void);

void phasesync_api(void);

void phasesync_atexit(void);

void phasesync_initialize(void);

void phasesync_terminate(void);

void phasesync_xil_shutdown(void);

void phasesync_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_phasesync_api.h
 *
 * [EOF]
 */
