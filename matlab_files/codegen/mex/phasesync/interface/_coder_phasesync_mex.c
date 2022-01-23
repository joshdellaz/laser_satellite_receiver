/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * _coder_phasesync_mex.c
 *
 * Code generation for function '_coder_phasesync_mex'
 *
 */

/* Include files */
#include "_coder_phasesync_mex.h"
#include "_coder_phasesync_api.h"
#include "phasesync_data.h"
#include "phasesync_initialize.h"
#include "phasesync_terminate.h"
#include "rt_nonfinite.h"

/* Function Definitions */
void mexFunction(int32_T nlhs, mxArray *plhs[], int32_T nrhs,
                 const mxArray *prhs[])
{
  (void)plhs;
  (void)prhs;
  mexAtExit(&phasesync_atexit);
  /* Module initialization. */
  phasesync_initialize();
  /* Dispatch the entry-point. */
  phasesync_mexFunction(nlhs, nrhs);
  /* Module termination. */
  phasesync_terminate();
}

emlrtCTX mexFunctionCreateRootTLS(void)
{
  emlrtCreateRootTLSR2021a(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1,
                           NULL);
  return emlrtRootTLSGlobal;
}

void phasesync_mexFunction(int32_T nlhs, int32_T nrhs)
{
  emlrtStack st = {
      NULL, /* site */
      NULL, /* tls */
      NULL  /* prev */
  };
  st.tls = emlrtRootTLSGlobal;
  /* Check for proper number of arguments. */
  if (nrhs != 0) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 0, 4,
                        9, "phasesync");
  }
  if (nlhs > 0) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 9,
                        "phasesync");
  }
  /* Call the function. */
  phasesync_api();
}

/* End of code generation (_coder_phasesync_mex.c) */
