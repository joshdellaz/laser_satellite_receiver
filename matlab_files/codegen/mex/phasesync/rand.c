/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * rand.c
 *
 * Code generation for function 'rand'
 *
 */

/* Include files */
#include "rand.h"
#include "rt_nonfinite.h"

/* Function Definitions */
void b_rand(real_T r[100])
{
  emlrtRandu(&r[0], 100);
}

/* End of code generation (rand.c) */
