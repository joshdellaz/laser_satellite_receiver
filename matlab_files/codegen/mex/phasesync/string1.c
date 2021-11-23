/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * string1.c
 *
 * Code generation for function 'string1'
 *
 */

/* Include files */
#include "string1.h"
#include "phasesync_data.h"
#include "phasesync_mexutil.h"
#include "rt_nonfinite.h"
#include <stdio.h>

/* Function Definitions */
void b_string_disp(const emlrtStack *sp)
{
  static const int32_T iv[2] = {1, 7};
  static const int32_T iv1[2] = {1, 4};
  static const int32_T iv2[2] = {1, 29};
  static const char_T varargin_1[29] = {
      'A', 'l', 'l', ' ', 'b', 'i', 't', 's', ' ', 's', 'u', 'c', 'c', 'e', 's',
      's', 'f', 'u', 'l', 'l', 'y', ' ', 's', 'a', 'm', 'p', 'l', 'e', 'd'};
  static const char_T u[7] = {'f', 'p', 'r', 'i', 'n', 't', 'f'};
  static const char_T b_u[4] = {'%', 's', '\\', 'n'};
  emlrtStack b_st;
  emlrtStack c_st;
  emlrtStack st;
  const mxArray *b_y;
  const mxArray *c_y;
  const mxArray *d_y;
  const mxArray *m;
  const mxArray *y;
  st.prev = sp;
  st.tls = sp->tls;
  st.site = &m_emlrtRSI;
  b_st.prev = &st;
  b_st.tls = st.tls;
  c_st.prev = &b_st;
  c_st.tls = b_st.tls;
  b_st.site = &n_emlrtRSI;
  y = NULL;
  m = emlrtCreateCharArray(2, &iv[0]);
  emlrtInitCharArrayR2013a(&b_st, 7, m, &u[0]);
  emlrtAssign(&y, m);
  b_y = NULL;
  m = emlrtCreateDoubleScalar(1.0);
  emlrtAssign(&b_y, m);
  c_y = NULL;
  m = emlrtCreateCharArray(2, &iv1[0]);
  emlrtInitCharArrayR2013a(&b_st, 4, m, &b_u[0]);
  emlrtAssign(&c_y, m);
  d_y = NULL;
  m = emlrtCreateCharArray(2, &iv2[0]);
  emlrtInitCharArrayR2013a(&b_st, 29, m, &varargin_1[0]);
  emlrtAssign(&d_y, m);
  c_st.site = &ob_emlrtRSI;
  emlrt_marshallIn(&c_st, feval(&c_st, y, b_y, c_y, d_y, &c_emlrtMCI),
                   "<output of feval>");
}

void c_string_disp(const emlrtStack *sp)
{
  static const int32_T iv[2] = {1, 7};
  static const int32_T iv1[2] = {1, 4};
  static const int32_T iv2[2] = {1, 21};
  static const char_T varargin_1[21] = {'U', 'n', 's', 'u', 'c', 'c', 'e',
                                        's', 's', 'f', 'u', 'l', ' ', 's',
                                        'a', 'm', 'p', 'l', 'i', 'n', 'g'};
  static const char_T u[7] = {'f', 'p', 'r', 'i', 'n', 't', 'f'};
  static const char_T b_u[4] = {'%', 's', '\\', 'n'};
  emlrtStack b_st;
  emlrtStack c_st;
  emlrtStack st;
  const mxArray *b_y;
  const mxArray *c_y;
  const mxArray *d_y;
  const mxArray *m;
  const mxArray *y;
  st.prev = sp;
  st.tls = sp->tls;
  st.site = &m_emlrtRSI;
  b_st.prev = &st;
  b_st.tls = st.tls;
  c_st.prev = &b_st;
  c_st.tls = b_st.tls;
  b_st.site = &n_emlrtRSI;
  y = NULL;
  m = emlrtCreateCharArray(2, &iv[0]);
  emlrtInitCharArrayR2013a(&b_st, 7, m, &u[0]);
  emlrtAssign(&y, m);
  b_y = NULL;
  m = emlrtCreateDoubleScalar(1.0);
  emlrtAssign(&b_y, m);
  c_y = NULL;
  m = emlrtCreateCharArray(2, &iv1[0]);
  emlrtInitCharArrayR2013a(&b_st, 4, m, &b_u[0]);
  emlrtAssign(&c_y, m);
  d_y = NULL;
  m = emlrtCreateCharArray(2, &iv2[0]);
  emlrtInitCharArrayR2013a(&b_st, 21, m, &varargin_1[0]);
  emlrtAssign(&d_y, m);
  c_st.site = &ob_emlrtRSI;
  emlrt_marshallIn(&c_st, feval(&c_st, y, b_y, c_y, d_y, &c_emlrtMCI),
                   "<output of feval>");
}

void string_disp(const emlrtStack *sp)
{
  static const int32_T iv[2] = {1, 7};
  static const int32_T iv1[2] = {1, 4};
  static const int32_T iv2[2] = {1, 19};
  static const char_T varargin_1[19] = {'D', 'e', 'c', 'o', 'd', 'e', 'd',
                                        ' ', 's', 'e', 'q', 'u', 'e', 'n',
                                        'c', 'e', ' ', '=', ' '};
  static const char_T u[7] = {'f', 'p', 'r', 'i', 'n', 't', 'f'};
  static const char_T b_u[4] = {'%', 's', '\\', 'n'};
  emlrtStack b_st;
  emlrtStack c_st;
  emlrtStack st;
  const mxArray *b_y;
  const mxArray *c_y;
  const mxArray *d_y;
  const mxArray *m;
  const mxArray *y;
  st.prev = sp;
  st.tls = sp->tls;
  st.site = &m_emlrtRSI;
  b_st.prev = &st;
  b_st.tls = st.tls;
  c_st.prev = &b_st;
  c_st.tls = b_st.tls;
  b_st.site = &n_emlrtRSI;
  y = NULL;
  m = emlrtCreateCharArray(2, &iv[0]);
  emlrtInitCharArrayR2013a(&b_st, 7, m, &u[0]);
  emlrtAssign(&y, m);
  b_y = NULL;
  m = emlrtCreateDoubleScalar(1.0);
  emlrtAssign(&b_y, m);
  c_y = NULL;
  m = emlrtCreateCharArray(2, &iv1[0]);
  emlrtInitCharArrayR2013a(&b_st, 4, m, &b_u[0]);
  emlrtAssign(&c_y, m);
  d_y = NULL;
  m = emlrtCreateCharArray(2, &iv2[0]);
  emlrtInitCharArrayR2013a(&b_st, 19, m, &varargin_1[0]);
  emlrtAssign(&d_y, m);
  c_st.site = &ob_emlrtRSI;
  emlrt_marshallIn(&c_st, feval(&c_st, y, b_y, c_y, d_y, &c_emlrtMCI),
                   "<output of feval>");
}

/* End of code generation (string1.c) */
