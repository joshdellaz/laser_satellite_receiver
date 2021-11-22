/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * _coder_phasesync_api.c
 *
 * Code generation for function '_coder_phasesync_api'
 *
 */

/* Include files */
#include "_coder_phasesync_api.h"
#include "phasesync.h"
#include "phasesync_data.h"
#include "rt_nonfinite.h"

/* Function Definitions */
void phasesync_api(void)
{
  emlrtStack st = {
      NULL, /* site */
      NULL, /* tls */
      NULL  /* prev */
  };
  st.tls = emlrtRootTLSGlobal;
  /* Invoke the target function */
  phasesync(&st);
}

/* End of code generation (_coder_phasesync_api.c) */
