#include <stdbool.h>
#include <stdint.h>

//Matlab code:
//%do the sampling
//size = 20+num_of_data_bits;
//output = zeros(1, size);
//selected_points_x = zeros(1, size);
//for i = 0:(size - 1)
//    if input_signal(round((i)*N*num_banks) + n_offset) >= 0 %rounding issues due to N?
//        output(i+1) = 1;
//        selected_points_x(i+1) = round((i)*N*num_banks) + n_offset;%for later infographic
//    else
//        output(i+1) = 0;
//        selected_points_x(i+1) = round((i)*N*num_banks) + n_offset;%for later infographic
//    end
//end

// static double rt_roundd_snf(double u)
// {
//     double y;
//     if (fabs(u) < 4.503599627370496E+15) {
//         if (u >= 0.5) {
//             y = floor(u + 0.5);
//         }
//         else if (u > -0.5) {
//             y = u * 0.0;
//         }
//         else {
//             y = ceil(u - 0.5);
//         }
//     }
//     else {
//         y = u;
//     }
//     return y;
// }

bool samplesToBytes(float* samples, int* num_bytes_to_convert, uint8_t* data, float phase_offset) {


    // /* use averaging? */
    // idx = (int)((float)rt_roundd_snf((double)idx / 4.0) * 4.0F +
    //     (float)fmod(idx, 4.0));
    // /* assuming 1 symbol = pi phase */
    // /* do the sampling */
    // for (Lg = 0; Lg < 120; Lg++) {
    //     if (y[((int)rt_roundd_snf((double)Lg * 4.2 * 4.0) + idx) + 8] >= 0.0) {
    //         /* rounding issues due to N? */
    //         output[Lg] = 1;
    //         /* for later infographic */
    //     }
    //     else {
    //         output[Lg] = 0;
    //         /* for later infographic */
    //     }
    // }
}