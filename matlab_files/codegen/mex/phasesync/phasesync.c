/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * phasesync.c
 *
 * Code generation for function 'phasesync'
 *
 */

/* Include files */
#include "phasesync.h"
#include "phasesync_data.h"
#include "phasesync_mexutil.h"
#include "rand.h"
#include "resample.h"
#include "rt_nonfinite.h"
#include "string1.h"
#include "sumMatrixIncludeNaN.h"
#include "mwmathutil.h"
#include <stdio.h>
#include <string.h>

/* Variable Definitions */
static emlrtRSInfo emlrtRSI =
    {
        23,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo b_emlrtRSI =
    {
        38,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo c_emlrtRSI =
    {
        48,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo d_emlrtRSI =
    {
        61,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo e_emlrtRSI =
    {
        63,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo f_emlrtRSI =
    {
        69,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo g_emlrtRSI =
    {
        97,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo h_emlrtRSI =
    {
        101,         /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo i_emlrtRSI =
    {
        103,         /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo j_emlrtRSI =
    {
        108,         /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo k_emlrtRSI =
    {
        110,         /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo l_emlrtRSI = {
    61,                                                            /* lineNo */
    "randi",                                                       /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\randfun\\randi.m" /* pathName
                                                                    */
};

static emlrtRSInfo r_emlrtRSI = {
    42,          /* lineNo */
    "circshift", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\elmat\\circshift.m" /* pathName
                                                                      */
};

static emlrtRSInfo s_emlrtRSI = {
    56,                     /* lineNo */
    "circshift_single_dim", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\elmat\\circshift.m" /* pathName
                                                                      */
};

static emlrtRSInfo t_emlrtRSI = {
    69,                     /* lineNo */
    "circshift_single_dim", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\elmat\\circshift.m" /* pathName
                                                                      */
};

static emlrtRSInfo u_emlrtRSI = {
    125,              /* lineNo */
    "circshift_core", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\elmat\\circshift.m" /* pathName
                                                                      */
};

static emlrtRSInfo v_emlrtRSI = {
    129,              /* lineNo */
    "circshift_core", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\elmat\\circshift.m" /* pathName
                                                                      */
};

static emlrtRSInfo w_emlrtRSI = {
    133,              /* lineNo */
    "circshift_core", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\elmat\\circshift.m" /* pathName
                                                                      */
};

static emlrtRSInfo x_emlrtRSI = {
    20,                                                          /* lineNo */
    "sum",                                                       /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\datafun\\sum.m" /* pathName */
};

static emlrtRSInfo y_emlrtRSI =
    {
        99,        /* lineNo */
        "sumprod", /* fcnName */
        "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\datafun\\private\\sumprod."
        "m" /* pathName */
};

static emlrtRSInfo ab_emlrtRSI =
    {
        74,                      /* lineNo */
        "combineVectorElements", /* fcnName */
        "D:\\MATLAB_"
        "2021\\toolbox\\eml\\lib\\matlab\\datafun\\private\\combineVectorElemen"
        "ts.m" /* pathName */
};

static emlrtRSInfo gb_emlrtRSI = {
    17,                                                          /* lineNo */
    "min",                                                       /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\lib\\matlab\\datafun\\min.m" /* pathName */
};

static emlrtRSInfo hb_emlrtRSI = {
    40,         /* lineNo */
    "minOrMax", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\eml\\+coder\\+internal\\minOrMax.m" /* pathName
                                                                         */
};

static emlrtRSInfo ib_emlrtRSI = {
    90,        /* lineNo */
    "minimum", /* fcnName */
    "D:\\MATLAB_2021\\toolbox\\eml\\eml\\+coder\\+internal\\minOrMax.m" /* pathName
                                                                         */
};

static emlrtMCInfo emlrtMCI =
    {
        39,          /* lineNo */
        1,           /* colNo */
        "phasesync", /* fName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pName */
};

static emlrtMCInfo b_emlrtMCI =
    {
        98,          /* lineNo */
        1,           /* colNo */
        "phasesync", /* fName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pName */
};

static emlrtDCInfo emlrtDCI = {
    80,          /* lineNo */
    8,           /* colNo */
    "phasesync", /* fName */
    "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_files\\phasesync."
    "m", /* pName */
    1    /* checkKind */
};

static emlrtBCInfo emlrtBCI = {
    1,              /* iFirst */
    2016,           /* iLast */
    80,             /* lineNo */
    8,              /* colNo */
    "input_signal", /* aName */
    "phasesync",    /* fName */
    "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_files\\phasesync."
    "m", /* pName */
    0    /* checkKind */
};

static emlrtRSInfo mb_emlrtRSI =
    {
        98,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

static emlrtRSInfo nb_emlrtRSI =
    {
        39,          /* lineNo */
        "phasesync", /* fcnName */
        "D:\\Documents\\GitHub\\laser_satellite_receiver\\matlab_"
        "files\\phasesync.m" /* pathName */
};

/* Function Declarations */
static void disp(const emlrtStack *sp, const mxArray *b, emlrtMCInfo *location);

/* Function Definitions */
static void disp(const emlrtStack *sp, const mxArray *b, emlrtMCInfo *location)
{
  const mxArray *pArray;
  pArray = b;
  emlrtCallMATLABR2012b((emlrtCTX)sp, 0, NULL, 1, &pArray,
                        (const char_T *)"disp", true, location);
}

void phasesync(const emlrtStack *sp)
{
  static const real_T fircoefs[49] = {
      -1.154575593983648E-17, -0.012730960239875706, -0.025712250513688212,
      -0.038094166414430156,  -0.048988416693311589, -0.057515241064219715,
      -0.062852167922348892,  -0.064281740012951435, -0.061235520866639484,
      -0.053331832980202207,  -0.040404965092938588, -0.022524006578241607,
      1.154575593983648E-17,  0.026619280501558328,  0.056566951130114,
      0.08888638830033703,    0.12247104173327902,   0.15611279717431065,
      0.18855650376704669,    0.21855791604403488,   0.244942083466558,
      0.266659164901011,      0.28283475565057009,   0.29281208551714133,
      0.29618386351056508,    0.29281208551714133,   0.28283475565057009,
      0.266659164901011,      0.244942083466558,     0.21855791604403488,
      0.18855650376704669,    0.15611279717431065,   0.12247104173327902,
      0.08888638830033703,    0.056566951130114,     0.026619280501558328,
      1.154575593983648E-17,  -0.022524006578241607, -0.040404965092938588,
      -0.053331832980202207,  -0.061235520866639484, -0.064281740012951435,
      -0.062852167922348892,  -0.057515241064219715, -0.048988416693311589,
      -0.038094166414430156,  -0.025712250513688212, -0.012730960239875706,
      -1.154575593983648E-17};
  static const int32_T iv1[2] = {1, 7};
  static const int32_T iv2[2] = {1, 4};
  static const int32_T iv3[2] = {1, 28};
  static const int32_T iv4[2] = {1, 7};
  static const int32_T iv5[2] = {1, 4};
  static const int32_T iv6[2] = {1, 33};
  static const int32_T iv7[2] = {1, 7};
  static const int32_T iv8[2] = {1, 4};
  static const int32_T iv9[2] = {1, 23};
  static const char_T b_varargin_1[33] = {
      'C', 'a', 'l', 'c', 'u', 'l', 'a', 't', 'e', 'd', ' ',
      'P', 'h', 'a', 's', 'e', ' ', 'o', 'f', 'f', 's', 'e',
      't', ' ', '(', 'r', 'a', 'd', 's', ')', ' ', '=', ' '};
  static const char_T varargin_1[28] = {
      'I', 'n', 'p', 'u', 't', ' ', 'P', 'h', 'a', 's', 'e', ' ', 'o', 'f',
      'f', 's', 'e', 't', ' ', '(', 'r', 'a', 'd', 's', ')', ' ', '=', ' '};
  static const char_T c_varargin_1[23] = {
      'G', 'e', 'n', 'e', 'r', 'a', 't', 'e', 'd', ' ', 'I', 'n',
      'p', 'u', 't', ' ', 'D', 'a', 't', 'a', ' ', '=', ' '};
  static const char_T c_u[7] = {'f', 'p', 'r', 'i', 'n', 't', 'f'};
  static const char_T e_u[7] = {'f', 'p', 'r', 'i', 'n', 't', 'f'};
  static const char_T u[7] = {'f', 'p', 'r', 'i', 'n', 't', 'f'};
  static const char_T b_u[4] = {'%', 's', '\\', 'n'};
  static const char_T d_u[4] = {'%', 's', '\\', 'n'};
  static const char_T f_u[4] = {'%', 's', '\\', 'n'};
  static const int8_T iv[20] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  emlrtStack b_st;
  emlrtStack c_st;
  emlrtStack d_st;
  emlrtStack st;
  const mxArray *b_y;
  const mxArray *c_y;
  const mxArray *d_y;
  const mxArray *e_y;
  const mxArray *f_y;
  const mxArray *g_y;
  const mxArray *h_y;
  const mxArray *i_y;
  const mxArray *j_y;
  const mxArray *k_y;
  const mxArray *l_y;
  const mxArray *m;
  const mxArray *m_y;
  const mxArray *n_y;
  const mxArray *y;
  real_T input_samples[5040];
  real_T input_signal[2016];
  real_T shiftedinput[2016];
  real_T a__1[1008];
  real_T b_input_samples[504];
  real_T input_data[120];
  real_T r[100];
  real_T bankout[49];
  real_T banksum[36];
  real_T d;
  real_T n_offset;
  int32_T b;
  int32_T i;
  int32_T idx;
  int32_T j;
  int32_T k;
  int32_T p_tmp;
  int8_T output[120];
  boolean_T b_p;
  boolean_T exitg1;
  boolean_T p;
  st.prev = sp;
  st.tls = sp->tls;
  b_st.prev = &st;
  b_st.tls = st.tls;
  c_st.prev = &b_st;
  c_st.tls = b_st.tls;
  d_st.prev = &c_st;
  d_st.tls = c_st.tls;
  /* TODO define/know actual meaning of phase offset */
  /* TODO implement bitrate shift as parameter */
  /* parameter definitions */
  /* clear */
  /* in rads */
  /* Can't have more than 1 decimal!!! Number of samples per bit */
  /* num of symbols of filter */
  /* num of samples in symbol period */
  /* generate square wave w/ 20 bit alternation and then random sequence */
  memset(&input_data[0], 0, 120U * sizeof(real_T));
  for (idx = 0; idx < 20; idx++) {
    input_data[idx] = iv[idx];
  }
  st.site = &emlrtRSI;
  b_st.site = &l_emlrtRSI;
  b_rand(r);
  for (k = 0; k < 100; k++) {
    r[k] = muDoubleScalarFloor(r[k] * 2.0);
  }
  memcpy(&input_data[20], &r[0], 100U * sizeof(real_T));
  /* Apply appropriate sample rate */
  idx = -1;
  for (k = 0; k < 120; k++) {
    for (j = 0; j < 42; j++) {
      input_samples[(idx + j) + 1] = input_data[k];
    }
    idx += 42;
  }
  /* if shift > half of sample, first sample is shifted by one */
  /*  %Print some useful stuff */
  st.site = &b_emlrtRSI;
  b_st.site = &m_emlrtRSI;
  c_st.site = &n_emlrtRSI;
  y = NULL;
  m = emlrtCreateCharArray(2, &iv1[0]);
  emlrtInitCharArrayR2013a(&c_st, 7, m, &u[0]);
  emlrtAssign(&y, m);
  b_y = NULL;
  m = emlrtCreateDoubleScalar(1.0);
  emlrtAssign(&b_y, m);
  c_y = NULL;
  m = emlrtCreateCharArray(2, &iv2[0]);
  emlrtInitCharArrayR2013a(&c_st, 4, m, &b_u[0]);
  emlrtAssign(&c_y, m);
  d_y = NULL;
  m = emlrtCreateCharArray(2, &iv3[0]);
  emlrtInitCharArrayR2013a(&c_st, 28, m, &varargin_1[0]);
  emlrtAssign(&d_y, m);
  d_st.site = &ob_emlrtRSI;
  emlrt_marshallIn(&d_st, feval(&d_st, y, b_y, c_y, d_y, &c_emlrtMCI),
                   "<output of feval>");
  e_y = NULL;
  m = emlrtCreateDoubleScalar(0.0);
  emlrtAssign(&e_y, m);
  st.site = &nb_emlrtRSI;
  disp(&st, e_y, &emlrtMCI);
  /*  figure */
  /*  tiledlayout(3,1) */
  /*  nexttile */
  /*  stem(input_samples(2:51)); */
  /*  title(["Output of ADC w/ Phase offset = " phase]); */
  /* correct waveform for proper processing */
  for (idx = 0; idx < 504; idx++) {
    b_input_samples[idx] = (input_samples[10 * idx] - 0.5) * 2.0;
  }
  st.site = &c_emlrtRSI;
  resample(&st, b_input_samples, input_signal);
  /*  %Print some useful stuff */
  /*  nexttile */
  /*  stem(input_signal(1:200)); */
  /*  title('Upsampled Signal'); */
  /* Create and apply polyphase filterbank */
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 4; j++) {
      st.site = &d_emlrtRSI;
      p_tmp = (i << 2) + j;
      memcpy(&shiftedinput[0], &input_signal[0], 2016U * sizeof(real_T));
      b_st.site = &r_emlrtRSI;
      c_st.site = &s_emlrtRSI;
      idx = p_tmp + 17;
      c_st.site = &t_emlrtRSI;
      memset(&a__1[0], 0, 1008U * sizeof(real_T));
      d_st.site = &u_emlrtRSI;
      if (0 <= idx) {
        memcpy(&a__1[0], &input_signal[0], (idx + 1) * sizeof(real_T));
      }
      b = 1997 - p_tmp;
      d_st.site = &v_emlrtRSI;
      for (k = 0; k <= b; k++) {
        shiftedinput[k] = shiftedinput[(k + p_tmp) + 18];
      }
      d_st.site = &w_emlrtRSI;
      for (k = 0; k <= idx; k++) {
        shiftedinput[(k - p_tmp) + 1998] = a__1[k];
      }
      for (k = 0; k < 49; k++) {
        bankout[k] = muDoubleScalarAbs(shiftedinput[k] * fircoefs[k]);
      }
      st.site = &e_emlrtRSI;
      b_st.site = &x_emlrtRSI;
      c_st.site = &y_emlrtRSI;
      d_st.site = &ab_emlrtRSI;
      banksum[p_tmp] = sumColumnB(bankout, 1);
      if (*emlrtBreakCheckR2012bFlagVar != 0) {
        emlrtBreakCheckR2012b((emlrtCTX)sp);
      }
    }
    if (*emlrtBreakCheckR2012bFlagVar != 0) {
      emlrtBreakCheckR2012b((emlrtCTX)sp);
    }
  }
  /* find phase offset */
  st.site = &f_emlrtRSI;
  b_st.site = &gb_emlrtRSI;
  c_st.site = &hb_emlrtRSI;
  d_st.site = &ib_emlrtRSI;
  if (!muDoubleScalarIsNaN(banksum[0])) {
    idx = 1;
  } else {
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k < 37)) {
      if (!muDoubleScalarIsNaN(banksum[k - 1])) {
        idx = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }
  if (idx == 0) {
    idx = 1;
  } else {
    n_offset = banksum[idx - 1];
    p_tmp = idx + 1;
    for (k = p_tmp; k < 37; k++) {
      d = banksum[k - 1];
      if (n_offset > d) {
        n_offset = d;
        idx = k;
      }
    }
  }
  /* use averaging? */
  n_offset = muDoubleScalarRound((real_T)idx / 4.0) * 4.0 +
             muDoubleScalarRem(idx, 4.0);
  /* assuming 1 symbol = pi phase */
  /* do the sampling */
  for (i = 0; i < 120; i++) {
    d = muDoubleScalarRound((real_T)i * 4.2 * 4.0) + n_offset;
    if (d != (int32_T)d) {
      emlrtIntegerCheckR2012b(d, &emlrtDCI, (emlrtCTX)sp);
    }
    if (((int32_T)d < 1) || ((int32_T)d > 2016)) {
      emlrtDynamicBoundsCheckR2012b((int32_T)d, 1, 2016, &emlrtBCI,
                                    (emlrtCTX)sp);
    }
    if (input_signal[(int32_T)d - 1] >= 0.0) {
      /* rounding issues due to N? */
      output[i] = 1;
      /* for later infographic */
    } else {
      output[i] = 0;
      /* for later infographic */
    }
    if (*emlrtBreakCheckR2012bFlagVar != 0) {
      emlrtBreakCheckR2012b((emlrtCTX)sp);
    }
  }
  /*  %plot selected sample point & print phase offset */
  /*  nexttile */
  /*  hold on  */
  /*  stem(input_signal(1:200)); */
  /*  stem(selected_points_x(1:12), (output(1:12)-0.5).*2, 'LineWidth',2); */
  /*  legend('Input signal (Oversampled)', 'Selected data sample points'); */
  /*  title('Optimal Sampling Points'); */
  /*  hold off */
  st.site = &g_emlrtRSI;
  b_st.site = &m_emlrtRSI;
  c_st.site = &n_emlrtRSI;
  f_y = NULL;
  m = emlrtCreateCharArray(2, &iv4[0]);
  emlrtInitCharArrayR2013a(&c_st, 7, m, &c_u[0]);
  emlrtAssign(&f_y, m);
  g_y = NULL;
  m = emlrtCreateDoubleScalar(1.0);
  emlrtAssign(&g_y, m);
  h_y = NULL;
  m = emlrtCreateCharArray(2, &iv5[0]);
  emlrtInitCharArrayR2013a(&c_st, 4, m, &d_u[0]);
  emlrtAssign(&h_y, m);
  i_y = NULL;
  m = emlrtCreateCharArray(2, &iv6[0]);
  emlrtInitCharArrayR2013a(&c_st, 33, m, &b_varargin_1[0]);
  emlrtAssign(&i_y, m);
  d_st.site = &ob_emlrtRSI;
  emlrt_marshallIn(&d_st, feval(&d_st, f_y, g_y, h_y, i_y, &c_emlrtMCI),
                   "<output of feval>");
  j_y = NULL;
  m = emlrtCreateDoubleScalar(muDoubleScalarAbs(
      n_offset / 16.0 * 3.1415926535897931 - 1.5707963267948966));
  emlrtAssign(&j_y, m);
  st.site = &mb_emlrtRSI;
  disp(&st, j_y, &b_emlrtMCI);
  /* print decoded sequence */
  st.site = &h_emlrtRSI;
  b_st.site = &m_emlrtRSI;
  c_st.site = &n_emlrtRSI;
  k_y = NULL;
  m = emlrtCreateCharArray(2, &iv7[0]);
  emlrtInitCharArrayR2013a(&c_st, 7, m, &e_u[0]);
  emlrtAssign(&k_y, m);
  l_y = NULL;
  m = emlrtCreateDoubleScalar(1.0);
  emlrtAssign(&l_y, m);
  m_y = NULL;
  m = emlrtCreateCharArray(2, &iv8[0]);
  emlrtInitCharArrayR2013a(&c_st, 4, m, &f_u[0]);
  emlrtAssign(&m_y, m);
  n_y = NULL;
  m = emlrtCreateCharArray(2, &iv9[0]);
  emlrtInitCharArrayR2013a(&c_st, 23, m, &c_varargin_1[0]);
  emlrtAssign(&n_y, m);
  d_st.site = &ob_emlrtRSI;
  emlrt_marshallIn(&d_st, feval(&d_st, k_y, l_y, m_y, n_y, &c_emlrtMCI),
                   "<output of feval>");
  /*  disp(num2str(input_data)); */
  st.site = &i_emlrtRSI;
  string_disp(&st);
  /*  disp(num2str(output)); */
  /* compare decoded sequence */
  p = true;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 120)) {
    if (!(input_data[k] == output[k])) {
      p = false;
      exitg1 = true;
    } else {
      k++;
    }
  }
  b_p = (int32_T)p;
  if (b_p) {
    st.site = &j_emlrtRSI;
    b_string_disp(&st);
  } else {
    st.site = &k_emlrtRSI;
    c_string_disp(&st);
  }
}

/* End of code generation (phasesync.c) */
