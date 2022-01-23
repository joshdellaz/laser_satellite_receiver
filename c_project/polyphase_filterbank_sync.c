#include <stdbool.h>

//MATLAB Generated Code

// bool resample(float* samples) {
//     static const double dv[18] = { 0.0,
//                                   -3.6599908762554245E-20,
//                                   -0.0018620382329331026,
//                                   -0.012833066896205812,
//                                   -0.029186818797226653,
//                                   1.2911673020744453E-17,
//                                   0.16615358664838167,
//                                   0.49942439609101541,
//                                   0.86491926089902982,
//                                   1.026769360575877,
//                                   0.86491926089902982,
//                                   0.49942439609101541,
//                                   0.16615358664838167,
//                                   1.2911673020744453E-17,
//                                   -0.029186818797226653,
//                                   -0.012833066896205812,
//                                   -0.0018620382329331026,
//                                   -3.6599908762554245E-20 };

//     /* correct waveform for proper processing & resample */
//     for (r = 0; r < 504; r++) {
//         varargin_1[r] = (input_samples[10 * r] - 0.5) * 2.0;
//     }
//     memset(&y[0], 0, 2030U * sizeof(double));
//     for (k = 0; k < 4; k++) {
//         out_idx = (double)k + 1.0;
//         if (k == 0) {
//             r = 0;
//         }
//         else {
//             r = k;
//         }
//         inphi_idx = 1.0;
//         filthi_idx = r + 1;
//         if (fmod(19.0 - ((double)r + 1.0), 4.0) != 0.0) {
//             Lg = (int)floor((19.0 - ((double)r + 1.0)) / 4.0 + 1.0);
//         }
//         else {
//             Lg = (int)floor((19.0 - ((double)r + 1.0)) / 4.0);
//         }
//         while ((inphi_idx < 505.0) && (filthi_idx < 19)) {
//             accum = 0.0;
//             idx = (int)inphi_idx;
//             for (tmp_filtlo_idx = r + 1; tmp_filtlo_idx <= filthi_idx;
//                 tmp_filtlo_idx += 4) {
//                 accum += dv[tmp_filtlo_idx - 1] * varargin_1[idx - 1];
//                 idx--;
//             }
//             y[(int)out_idx - 1] += accum;
//             out_idx += 4.0;
//             inphi_idx++;
//             filthi_idx += 4;
//         }
//         if (inphi_idx < 505.0) {
//             while (filthi_idx >= 19) {
//                 filthi_idx -= 4;
//             }
//             while (inphi_idx < 505.0) {
//                 accum = 0.0;
//                 idx = (int)inphi_idx;
//                 for (tmp_filtlo_idx = r + 1; tmp_filtlo_idx <= filthi_idx;
//                     tmp_filtlo_idx += 4) {
//                     accum += dv[tmp_filtlo_idx - 1] * varargin_1[idx - 1];
//                     idx--;
//                 }
//                 if (out_idx < 2031.0) {
//                     idx = (int)out_idx - 1;
//                     y[idx] += accum;
//                 }
//                 out_idx += 4.0;
//                 inphi_idx++;
//             }
//         }
//         else if (filthi_idx < 19) {
//             while (filthi_idx < 19) {
//                 accum = 0.0;
//                 for (idx = 0; idx < 504; idx++) {
//                     accum += dv[(filthi_idx + idx * -4) - 1] * varargin_1[idx];
//                 }
//                 y[(int)out_idx - 1] += accum;
//                 out_idx += 4.0;
//                 inphi_idx++;
//                 filthi_idx += 4;
//             }
//         }
//         while (filthi_idx >= 19) {
//             filthi_idx -= 4;
//         }
//         for (inplo_idx = (inphi_idx - (double)Lg) + 1.0; inplo_idx < 505.0;
//             inplo_idx++) {
//             accum = 0.0;
//             idx = (int)inplo_idx;
//             inphi_idx = filthi_idx;
//             while (idx < 505) {
//                 accum += dv[(int)inphi_idx - 1] * varargin_1[idx - 1];
//                 idx++;
//                 inphi_idx -= 4.0;
//             }
//             y[(int)out_idx - 1] += accum;
//             out_idx += 4.0;
//         }
//     }
// }

// bool determinePhaseOffset(float* samples, float phase_offset)//design such that it can be done on 101010... stream OR w/ MLS sync
// {
//     static const double fircoefs[49] = {
//         -1.154575593983648E-17, -0.012730960239875706, -0.025712250513688212,
//         -0.038094166414430156,  -0.048988416693311589, -0.057515241064219715,
//         -0.062852167922348892,  -0.064281740012951435, -0.061235520866639484,
//         -0.053331832980202207,  -0.040404965092938588, -0.022524006578241607,
//         1.154575593983648E-17,  0.026619280501558328,  0.056566951130114,
//         0.08888638830033703,    0.12247104173327902,   0.15611279717431065,
//         0.18855650376704669,    0.21855791604403488,   0.244942083466558,
//         0.266659164901011,      0.28283475565057009,   0.29281208551714133,
//         0.29618386351056508,    0.29281208551714133,   0.28283475565057009,
//         0.266659164901011,      0.244942083466558,     0.21855791604403488,
//         0.18855650376704669,    0.15611279717431065,   0.12247104173327902,
//         0.08888638830033703,    0.056566951130114,     0.026619280501558328,
//         1.154575593983648E-17,  -0.022524006578241607, -0.040404965092938588,
//         -0.053331832980202207,  -0.061235520866639484, -0.064281740012951435,
//         -0.062852167922348892,  -0.057515241064219715, -0.048988416693311589,
//         -0.038094166414430156,  -0.025712250513688212, -0.012730960239875706,
//         -1.154575593983648E-17 };
    
//     static const signed char iv[20] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
//                                        0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
//     double input_samples[5040];
//     double y[2030];
//     double shiftedinput[2016];
//     double a__1[1008];
//     double varargin_1[504];
//     double input_data[120];
//     double b_r[100];
//     double bankout[49];
//     double banksum[36];
//     double accum;
//     double inphi_idx;
//     double inplo_idx;
//     double out_idx;
//     int Lg;
//     int filthi_idx;
//     int idx;
//     int k;
//     int r;
//     int tmp_filtlo_idx;
//     signed char output[120];
//     bool b_p;
//     bool exitg1;
//     bool p;
//     /* parameter definitions */
//     /* clear */
//     /* in rads */
//     /* Can't have more than 1 decimal!!! Number of samples per bit */
//     /* num of symbols of filter */
//     /* num of samples in symbol period */
//     /* generate square wave w/ 20 bit alternation and then random sequence */
//     memset(&input_data[0], 0, 120U * sizeof(double));
//     for (r = 0; r < 20; r++) {
//         input_data[r] = iv[r];
//     }
//     b_rand(b_r);
//     for (k = 0; k < 100; k++) {
//         inphi_idx = floor(b_r[k] * 2.0);
//         b_r[k] = inphi_idx;
//         input_data[k + 20] = inphi_idx;
//     }
//     /* Apply appropriate sample rate */
//     idx = -1;
//     for (k = 0; k < 120; k++) {
//         for (filthi_idx = 0; filthi_idx < 42; filthi_idx++) {
//             input_samples[(idx + filthi_idx) + 1] = input_data[k];
//         }
//         idx += 42;
//     }
//     /* if shift > half of sample, first sample is shifted by one */


   
//     /* Create and apply polyphase filterbank */
//     for (Lg = 0; Lg < 9; Lg++) {
//         for (filthi_idx = 0; filthi_idx < 4; filthi_idx++) {
//             idx = (Lg << 2) + filthi_idx;
//             memcpy(&shiftedinput[0], &y[9], 2016U * sizeof(double));
//             tmp_filtlo_idx = idx + 17;
//             memset(&a__1[0], 0, 1008U * sizeof(double));
//             if (0 <= tmp_filtlo_idx) {
//                 memcpy(&a__1[0], &y[9], (tmp_filtlo_idx + 1) * sizeof(double));
//             }
//             r = 1997 - idx;
//             for (k = 0; k <= r; k++) {
//                 shiftedinput[k] = shiftedinput[(k + idx) + 18];
//             }
//             for (k = 0; k <= tmp_filtlo_idx; k++) {
//                 shiftedinput[(k - idx) + 1998] = a__1[k];
//             }
//             for (k = 0; k < 49; k++) {
//                 bankout[k] = fabs(shiftedinput[k] * fircoefs[k]);
//             }
//             inphi_idx = bankout[0];
//             for (k = 0; k < 48; k++) {
//                 inphi_idx += bankout[k + 1];
//             }
//             banksum[idx] = inphi_idx;
//         }
//     }
//     /* find phase offset */
//     if (!rtIsNaN(banksum[0])) {
//         idx = 1;
//     }
//     else {
//         idx = 0;
//         k = 2;
//         exitg1 = false;
//         while ((!exitg1) && (k < 37)) {
//             if (!rtIsNaN(banksum[k - 1])) {
//                 idx = k;
//                 exitg1 = true;
//             }
//             else {
//                 k++;
//             }
//         }
//     }
//     if (idx == 0) {
//         idx = 1;
//     }
//     else {
//         inplo_idx = banksum[idx - 1];
//         r = idx + 1;
//         for (k = r; k < 37; k++) {
//             inphi_idx = banksum[k - 1];
//             if (inplo_idx > inphi_idx) {
//                 inplo_idx = inphi_idx;
//                 idx = k;
//             }
//         }
//     }
// }