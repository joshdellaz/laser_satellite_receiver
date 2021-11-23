/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * resample.c
 *
 * Code generation for function 'resample'
 *
 */

/* Include files */
#include "resample.h"
#include "rt_nonfinite.h"
#include "mwmathutil.h"
#include <string.h>

/* Variable Definitions */
static emlrtRSInfo o_emlrtRSI = {
    220,                                                   /* lineNo */
    "resample",                                            /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\signal\\signal\\resample.m" /* pathName */
};

static emlrtRSInfo p_emlrtRSI = {
    485,                                                   /* lineNo */
    "uniformResample",                                     /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\signal\\signal\\resample.m" /* pathName */
};

static emlrtBCInfo b_emlrtBCI = {
    1,                                                  /* iFirst */
    2030,                                               /* iLast */
    129,                                                /* lineNo */
    13,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    3                                                   /* checkKind */
};

static emlrtBCInfo c_emlrtBCI = {
    1,                                                  /* iFirst */
    504,                                                /* iLast */
    125,                                                /* lineNo */
    58,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    0                                                   /* checkKind */
};

static emlrtBCInfo d_emlrtBCI = {
    1,                                                  /* iFirst */
    2030,                                               /* iLast */
    178,                                                /* lineNo */
    17,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    3                                                   /* checkKind */
};

static emlrtBCInfo e_emlrtBCI = {
    1,                                                  /* iFirst */
    18,                                                 /* iLast */
    174,                                                /* lineNo */
    38,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    0                                                   /* checkKind */
};

static emlrtBCInfo f_emlrtBCI = {
    1,                                                  /* iFirst */
    504,                                                /* iLast */
    151,                                                /* lineNo */
    62,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    0                                                   /* checkKind */
};

static emlrtBCInfo g_emlrtBCI = {
    1,                                                  /* iFirst */
    2030,                                               /* iLast */
    204,                                                /* lineNo */
    13,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    3                                                   /* checkKind */
};

static emlrtBCInfo h_emlrtBCI = {
    1,                                                  /* iFirst */
    18,                                                 /* iLast */
    200,                                                /* lineNo */
    34,                                                 /* colNo */
    "",                                                 /* aName */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    0                                                   /* checkKind */
};

static emlrtDCInfo b_emlrtDCI = {
    200,                                                /* lineNo */
    34,                                                 /* colNo */
    "upfirdn",                                          /* fName */
    "D:\\MATLAB_2021\\toolbox\\signal\\eml\\upfirdn.m", /* pName */
    1                                                   /* checkKind */
};

/* Function Definitions */
void resample(const emlrtStack *sp, const real_T varargin_1[504],
              real_T varargout_1[2016])
{
  static const real_T dv[18] = {0.0,
                                -3.6599908762554245E-20,
                                -0.0018620382329331026,
                                -0.012833066896205812,
                                -0.029186818797226653,
                                1.2911673020744453E-17,
                                0.16615358664838167,
                                0.49942439609101541,
                                0.86491926089902982,
                                1.026769360575877,
                                0.86491926089902982,
                                0.49942439609101541,
                                0.16615358664838167,
                                1.2911673020744453E-17,
                                -0.029186818797226653,
                                -0.012833066896205812,
                                -0.0018620382329331026,
                                -3.6599908762554245E-20};
  emlrtStack b_st;
  emlrtStack st;
  real_T y[2030];
  real_T accum;
  real_T inphi_idx;
  real_T out_idx;
  real_T tmp_filthi_idx;
  int32_T Lg;
  int32_T b_r;
  int32_T filthi_idx;
  int32_T r;
  int32_T tmp_inphi_idx;
  int32_T tmp_inplo_idx;
  st.prev = sp;
  st.tls = sp->tls;
  st.site = &o_emlrtRSI;
  b_st.prev = &st;
  b_st.tls = st.tls;
  b_st.site = &p_emlrtRSI;
  memset(&y[0], 0, 2030U * sizeof(real_T));
  for (r = 0; r < 4; r++) {
    out_idx = (real_T)r + 1.0;
    if (r == 0) {
      b_r = 0;
    } else {
      b_r = r;
    }
    inphi_idx = 1.0;
    filthi_idx = b_r + 1;
    if (muDoubleScalarRem(19.0 - ((real_T)b_r + 1.0), 4.0) != 0.0) {
      Lg = (int32_T)muDoubleScalarFloor((19.0 - ((real_T)b_r + 1.0)) / 4.0 +
                                        1.0);
    } else {
      Lg = (int32_T)muDoubleScalarFloor((19.0 - ((real_T)b_r + 1.0)) / 4.0);
    }
    while ((inphi_idx < 505.0) && (filthi_idx < 19)) {
      accum = 0.0;
      tmp_inphi_idx = (int32_T)inphi_idx;
      for (tmp_inplo_idx = b_r + 1; tmp_inplo_idx <= filthi_idx;
           tmp_inplo_idx += 4) {
        if (tmp_inphi_idx < 1) {
          emlrtDynamicBoundsCheckR2012b(0, 1, 504, &c_emlrtBCI, &b_st);
        }
        accum += dv[tmp_inplo_idx - 1] * varargin_1[tmp_inphi_idx - 1];
        tmp_inphi_idx--;
      }
      if (((int32_T)out_idx < 1) || ((int32_T)out_idx > 2030)) {
        emlrtDynamicBoundsCheckR2012b((int32_T)out_idx, 1, 2030, &b_emlrtBCI,
                                      &b_st);
      }
      y[(int32_T)out_idx - 1] += accum;
      out_idx += 4.0;
      inphi_idx++;
      filthi_idx += 4;
    }
    if (inphi_idx < 505.0) {
      while (filthi_idx >= 19) {
        filthi_idx -= 4;
      }
      while (inphi_idx < 505.0) {
        accum = 0.0;
        tmp_inphi_idx = (int32_T)inphi_idx;
        for (tmp_inplo_idx = b_r + 1; tmp_inplo_idx <= filthi_idx;
             tmp_inplo_idx += 4) {
          if (tmp_inphi_idx < 1) {
            emlrtDynamicBoundsCheckR2012b(0, 1, 504, &f_emlrtBCI, &b_st);
          }
          accum += dv[tmp_inplo_idx - 1] * varargin_1[tmp_inphi_idx - 1];
          tmp_inphi_idx--;
        }
        if (out_idx < 2031.0) {
          tmp_inphi_idx = (int32_T)out_idx - 1;
          y[tmp_inphi_idx] += accum;
        }
        out_idx += 4.0;
        inphi_idx++;
      }
    } else if (filthi_idx < 19) {
      while (filthi_idx < 19) {
        accum = 0.0;
        for (tmp_inplo_idx = 0; tmp_inplo_idx < 504; tmp_inplo_idx++) {
          tmp_inphi_idx = filthi_idx + tmp_inplo_idx * -4;
          if (tmp_inphi_idx < 1) {
            emlrtDynamicBoundsCheckR2012b(tmp_inphi_idx, 1, 18, &e_emlrtBCI,
                                          &b_st);
          }
          accum += dv[tmp_inphi_idx - 1] * varargin_1[tmp_inplo_idx];
        }
        if (((int32_T)out_idx < 1) || ((int32_T)out_idx > 2030)) {
          emlrtDynamicBoundsCheckR2012b((int32_T)out_idx, 1, 2030, &d_emlrtBCI,
                                        &b_st);
        }
        y[(int32_T)out_idx - 1] += accum;
        out_idx += 4.0;
        inphi_idx++;
        filthi_idx += 4;
      }
    }
    while (filthi_idx >= 19) {
      filthi_idx -= 4;
    }
    for (inphi_idx = (inphi_idx - (real_T)Lg) + 1.0; inphi_idx < 505.0;
         inphi_idx++) {
      accum = 0.0;
      tmp_inplo_idx = (int32_T)inphi_idx;
      tmp_filthi_idx = filthi_idx;
      while (tmp_inplo_idx < 505) {
        if (tmp_filthi_idx != (int32_T)tmp_filthi_idx) {
          emlrtIntegerCheckR2012b(tmp_filthi_idx, &b_emlrtDCI, &b_st);
        }
        if (((int32_T)tmp_filthi_idx < 1) || ((int32_T)tmp_filthi_idx > 18)) {
          emlrtDynamicBoundsCheckR2012b((int32_T)tmp_filthi_idx, 1, 18,
                                        &h_emlrtBCI, &b_st);
        }
        accum +=
            dv[(int32_T)tmp_filthi_idx - 1] * varargin_1[tmp_inplo_idx - 1];
        tmp_inplo_idx++;
        tmp_filthi_idx -= 4.0;
      }
      if (((int32_T)out_idx < 1) || ((int32_T)out_idx > 2030)) {
        emlrtDynamicBoundsCheckR2012b((int32_T)out_idx, 1, 2030, &g_emlrtBCI,
                                      &b_st);
      }
      y[(int32_T)out_idx - 1] += accum;
      out_idx += 4.0;
    }
  }
  memcpy(&varargout_1[0], &y[9], 2016U * sizeof(real_T));
}

/* End of code generation (resample.c) */
