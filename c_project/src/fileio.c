#include <stdint.h>
#include <stdio.h>

// int readFiletoArray(char * filename, uint8_t * data, int * len_data){

//     char c;
//     len_data = 0;
//     FILE *file;
//     file = fopen(*filename, "r");
//     if (file) {
//         while ((c = (char)getc(file)) != EOF)
//             data[*len_data] = c;
//             *len_data++;
//         fclose(file);
//     }
// }

// //ensure filename has an extension
// int writeArraytoFile(char * filename, uint8_t * data, int len_data){

//     char c;
//     FILE *file;
//     file = fopen(*filename, "w");
//     if (file) {
//         fwrite(data, sizeof(char), len_data, file);
//         fclose(file);
//     }
// }