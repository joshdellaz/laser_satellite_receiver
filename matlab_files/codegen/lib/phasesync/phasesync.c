/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: phasesync.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 15-Nov-2021 17:51:29
 */

/* Include Files */
#include "phasesync.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Variable Definitions */
static unsigned int method;

static unsigned int state;

static unsigned int b_state[2];

static boolean_T state_not_empty;

static boolean_T isInitialized_phasesync = false;

/* Function Declarations */
static void b_rand(double r[100]);

static void eml_rand_init(void);

static void eml_rand_mcg16807_stateful_init(void);

static void eml_rand_shr3cong_stateful_init(void);

static double rt_roundd_snf(double u);

/* Function Definitions */
/*
 * Arguments    : double r[100]
 * Return Type  : void
 */
static void b_rand(double r[100])
{
  static unsigned int c_state[625];
  double c_r;
  unsigned int u[2];
  unsigned int b_r;
  int hi;
  int k;
  int kk;
  unsigned int y;
  if (method == 4U) {
    for (k = 0; k < 100; k++) {
      hi = (int)(state / 127773U);
      b_r = 16807U * (state - hi * 127773U);
      y = 2836U * hi;
      if (b_r < y) {
        state = ~(y - b_r) & 2147483647U;
      } else {
        state = b_r - y;
      }
      r[k] = (double)state * 4.6566128752457969E-10;
    }
  } else if (method == 5U) {
    for (k = 0; k < 100; k++) {
      b_r = 69069U * b_state[0] + 1234567U;
      y = b_state[1] ^ b_state[1] << 13;
      y ^= y >> 17;
      y ^= y << 5;
      b_state[0] = b_r;
      b_state[1] = y;
      r[k] = (double)(b_r + y) * 2.328306436538696E-10;
    }
  } else {
    if (!state_not_empty) {
      memset(&c_state[0], 0, 625U * sizeof(unsigned int));
      b_r = 5489U;
      c_state[0] = 5489U;
      for (hi = 0; hi < 623; hi++) {
        b_r = ((b_r ^ b_r >> 30U) * 1812433253U + hi) + 1U;
        c_state[hi + 1] = b_r;
      }
      c_state[624] = 624U;
      state_not_empty = true;
    }
    for (k = 0; k < 100; k++) {
      /* ========================= COPYRIGHT NOTICE ============================
       */
      /*  This is a uniform (0,1) pseudorandom number generator based on: */
      /*                                                                         */
      /*  A C-program for MT19937, with initialization improved 2002/1/26. */
      /*  Coded by Takuji Nishimura and Makoto Matsumoto. */
      /*                                                                         */
      /*  Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura, */
      /*  All rights reserved. */
      /*                                                                         */
      /*  Redistribution and use in source and binary forms, with or without */
      /*  modification, are permitted provided that the following conditions */
      /*  are met: */
      /*                                                                         */
      /*    1. Redistributions of source code must retain the above copyright */
      /*       notice, this list of conditions and the following disclaimer. */
      /*                                                                         */
      /*    2. Redistributions in binary form must reproduce the above copyright
       */
      /*       notice, this list of conditions and the following disclaimer */
      /*       in the documentation and/or other materials provided with the */
      /*       distribution. */
      /*                                                                         */
      /*    3. The names of its contributors may not be used to endorse or */
      /*       promote products derived from this software without specific */
      /*       prior written permission. */
      /*                                                                         */
      /*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS */
      /*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT */
      /*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
       */
      /*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
       */
      /*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
       */
      /*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
      /*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
       */
      /*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
       */
      /*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT */
      /*  (INCLUDING  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
       */
      /*  OF THIS  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
       */
      /*                                                                         */
      /* =============================   END   =================================
       */
      do {
        for (hi = 0; hi < 2; hi++) {
          b_r = c_state[624] + 1U;
          if (c_state[624] + 1U >= 625U) {
            for (kk = 0; kk < 227; kk++) {
              y = (c_state[kk] & 2147483648U) | (c_state[kk + 1] & 2147483647U);
              if ((y & 1U) == 0U) {
                y >>= 1U;
              } else {
                y = y >> 1U ^ 2567483615U;
              }
              c_state[kk] = c_state[kk + 397] ^ y;
            }
            for (kk = 0; kk < 396; kk++) {
              y = (c_state[kk + 227] & 2147483648U) |
                  (c_state[kk + 228] & 2147483647U);
              if ((y & 1U) == 0U) {
                y >>= 1U;
              } else {
                y = y >> 1U ^ 2567483615U;
              }
              c_state[kk + 227] = c_state[kk] ^ y;
            }
            y = (c_state[623] & 2147483648U) | (c_state[0] & 2147483647U);
            if ((y & 1U) == 0U) {
              y >>= 1U;
            } else {
              y = y >> 1U ^ 2567483615U;
            }
            c_state[623] = c_state[396] ^ y;
            b_r = 1U;
          }
          y = c_state[(int)b_r - 1];
          c_state[624] = b_r;
          y ^= y >> 11U;
          y ^= y << 7U & 2636928640U;
          y ^= y << 15U & 4022730752U;
          u[hi] = y ^ y >> 18U;
        }
        u[0] >>= 5U;
        u[1] >>= 6U;
        c_r = 1.1102230246251565E-16 *
              ((double)u[0] * 6.7108864E+7 + (double)u[1]);
      } while (c_r == 0.0);
      r[k] = c_r;
    }
  }
}

/*
 * Arguments    : void
 * Return Type  : void
 */
static void eml_rand_init(void)
{
  method = 7U;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
static void eml_rand_mcg16807_stateful_init(void)
{
  state = 1144108930U;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
static void eml_rand_shr3cong_stateful_init(void)
{
  b_state[0] = 362436069U;
  b_state[1] = 521288629U;
}

/*
 * Arguments    : double u
 * Return Type  : double
 */
static double rt_roundd_snf(double u)
{
  double y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }
  return y;
}

/*
 * TODO define/know actual meaning of phase offset
 * TODO implement bitrate shift as parameter
 *
 * Arguments    : void
 * Return Type  : void
 */
void phasesync(void)
{
  static const double fircoefs[49] = {
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
  static const double dv[18] = {0.0,
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
  static const signed char iv[20] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                     0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  double input_samples[5040];
  double y[2030];
  double shiftedinput[2016];
  double a__1[1008];
  double varargin_1[504];
  double input_data[120];
  double b_r[100];
  double bankout[49];
  double banksum[36];
  double accum;
  double inphi_idx;
  double inplo_idx;
  double out_idx;
  int Lg;
  int filthi_idx;
  int idx;
  int k;
  int r;
  int tmp_filtlo_idx;
  signed char output[120];
  boolean_T b_p;
  boolean_T exitg1;
  boolean_T p;
  if (!isInitialized_phasesync) {
    phasesync_initialize();
  }
  /* parameter definitions */
  /* clear */
  /* in rads */
  /* Can't have more than 1 decimal!!! Number of samples per bit */
  /* num of symbols of filter */
  /* num of samples in symbol period */
  /* generate square wave w/ 20 bit alternation and then random sequence */
  memset(&input_data[0], 0, 120U * sizeof(double));
  for (r = 0; r < 20; r++) {
    input_data[r] = iv[r];
  }
  b_rand(b_r);
  for (k = 0; k < 100; k++) {
    inphi_idx = floor(b_r[k] * 2.0);
    b_r[k] = inphi_idx;
    input_data[k + 20] = inphi_idx;
  }
  /* Apply appropriate sample rate */
  idx = -1;
  for (k = 0; k < 120; k++) {
    for (filthi_idx = 0; filthi_idx < 42; filthi_idx++) {
      input_samples[(idx + filthi_idx) + 1] = input_data[k];
    }
    idx += 42;
  }
  /* if shift > half of sample, first sample is shifted by one */
  /*  %Print some useful stuff */
  printf("%s\n", "Input Phase offset (rads) = ");
  fflush(stdout);
  /*  figure */
  /*  tiledlayout(3,1) */
  /*  nexttile */
  /*  stem(input_samples(2:51)); */
  /*  title(["Output of ADC w/ Phase offset = " phase]); */
  /* correct waveform for proper processing */
  for (r = 0; r < 504; r++) {
    varargin_1[r] = (input_samples[10 * r] - 0.5) * 2.0;
  }
  memset(&y[0], 0, 2030U * sizeof(double));
  for (k = 0; k < 4; k++) {
    out_idx = (double)k + 1.0;
    if (k == 0) {
      r = 0;
    } else {
      r = k;
    }
    inphi_idx = 1.0;
    filthi_idx = r + 1;
    if (fmod(19.0 - ((double)r + 1.0), 4.0) != 0.0) {
      Lg = (int)floor((19.0 - ((double)r + 1.0)) / 4.0 + 1.0);
    } else {
      Lg = (int)floor((19.0 - ((double)r + 1.0)) / 4.0);
    }
    while ((inphi_idx < 505.0) && (filthi_idx < 19)) {
      accum = 0.0;
      idx = (int)inphi_idx;
      for (tmp_filtlo_idx = r + 1; tmp_filtlo_idx <= filthi_idx;
           tmp_filtlo_idx += 4) {
        accum += dv[tmp_filtlo_idx - 1] * varargin_1[idx - 1];
        idx--;
      }
      y[(int)out_idx - 1] += accum;
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
        idx = (int)inphi_idx;
        for (tmp_filtlo_idx = r + 1; tmp_filtlo_idx <= filthi_idx;
             tmp_filtlo_idx += 4) {
          accum += dv[tmp_filtlo_idx - 1] * varargin_1[idx - 1];
          idx--;
        }
        if (out_idx < 2031.0) {
          idx = (int)out_idx - 1;
          y[idx] += accum;
        }
        out_idx += 4.0;
        inphi_idx++;
      }
    } else if (filthi_idx < 19) {
      while (filthi_idx < 19) {
        accum = 0.0;
        for (idx = 0; idx < 504; idx++) {
          accum += dv[(filthi_idx + idx * -4) - 1] * varargin_1[idx];
        }
        y[(int)out_idx - 1] += accum;
        out_idx += 4.0;
        inphi_idx++;
        filthi_idx += 4;
      }
    }
    while (filthi_idx >= 19) {
      filthi_idx -= 4;
    }
    for (inplo_idx = (inphi_idx - (double)Lg) + 1.0; inplo_idx < 505.0;
         inplo_idx++) {
      accum = 0.0;
      idx = (int)inplo_idx;
      inphi_idx = filthi_idx;
      while (idx < 505) {
        accum += dv[(int)inphi_idx - 1] * varargin_1[idx - 1];
        idx++;
        inphi_idx -= 4.0;
      }
      y[(int)out_idx - 1] += accum;
      out_idx += 4.0;
    }
  }
  /*  %Print some useful stuff */
  /*  nexttile */
  /*  stem(input_signal(1:200)); */
  /*  title('Upsampled Signal'); */
  /* Create and apply polyphase filterbank */
  for (Lg = 0; Lg < 9; Lg++) {
    for (filthi_idx = 0; filthi_idx < 4; filthi_idx++) {
      idx = (Lg << 2) + filthi_idx;
      memcpy(&shiftedinput[0], &y[9], 2016U * sizeof(double));
      tmp_filtlo_idx = idx + 17;
      memset(&a__1[0], 0, 1008U * sizeof(double));
      if (0 <= tmp_filtlo_idx) {
        memcpy(&a__1[0], &y[9], (tmp_filtlo_idx + 1) * sizeof(double));
      }
      r = 1997 - idx;
      for (k = 0; k <= r; k++) {
        shiftedinput[k] = shiftedinput[(k + idx) + 18];
      }
      for (k = 0; k <= tmp_filtlo_idx; k++) {
        shiftedinput[(k - idx) + 1998] = a__1[k];
      }
      for (k = 0; k < 49; k++) {
        bankout[k] = fabs(shiftedinput[k] * fircoefs[k]);
      }
      inphi_idx = bankout[0];
      for (k = 0; k < 48; k++) {
        inphi_idx += bankout[k + 1];
      }
      banksum[idx] = inphi_idx;
    }
  }
  /* find phase offset */
  if (!rtIsNaN(banksum[0])) {
    idx = 1;
  } else {
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k < 37)) {
      if (!rtIsNaN(banksum[k - 1])) {
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
    inplo_idx = banksum[idx - 1];
    r = idx + 1;
    for (k = r; k < 37; k++) {
      inphi_idx = banksum[k - 1];
      if (inplo_idx > inphi_idx) {
        inplo_idx = inphi_idx;
        idx = k;
      }
    }
  }
  /* use averaging? */
  idx = (int)((float)rt_roundd_snf((double)idx / 4.0) * 4.0F +
              (float)fmod(idx, 4.0));
  /* assuming 1 symbol = pi phase */
  /* do the sampling */
  for (Lg = 0; Lg < 120; Lg++) {
    if (y[((int)rt_roundd_snf((double)Lg * 4.2 * 4.0) + idx) + 8] >= 0.0) {
      /* rounding issues due to N? */
      output[Lg] = 1;
      /* for later infographic */
    } else {
      output[Lg] = 0;
      /* for later infographic */
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
  printf("%s\n", "Calculated Phase offset (rads) = ");
  fflush(stdout);
  /* print decoded sequence */
  printf("%s\n", "Generated Input Data = ");
  fflush(stdout);
  /*  disp(num2str(input_data)); */
  printf("%s\n", "Decoded sequence = ");
  fflush(stdout);
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
  b_p = (int)p;
  if (b_p) {
    printf("%s\n", "All bits successfully sampled");
    fflush(stdout);
  } else {
    printf("%s\n", "Unsuccessful sampling");
    fflush(stdout);
  }
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void phasesync_initialize(void)
{
  state_not_empty = false;
  eml_rand_init();
  eml_rand_mcg16807_stateful_init();
  eml_rand_shr3cong_stateful_init();
  isInitialized_phasesync = true;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void phasesync_terminate(void)
{
  /* (no terminate code required) */
  isInitialized_phasesync = false;
}

/*
 * File trailer for phasesync.c
 *
 * [EOF]
 */
