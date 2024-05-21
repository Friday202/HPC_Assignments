#include <stdlib.h>
#include "orbium.h"

uint8_t inferno_pallete[] = {/* palette */
                             0, 0, 4,
                             1, 0, 5,
                             1, 1, 6,
                             1, 1, 8,
                             2, 1, 10,
                             2, 2, 12,
                             2, 2, 14,
                             3, 2, 16,
                             4, 3, 18,
                             4, 3, 20,
                             5, 4, 23,
                             6, 4, 25,
                             7, 5, 27,
                             8, 5, 29,
                             9, 6, 31,
                             10, 7, 34,
                             11, 7, 36,
                             12, 8, 38,
                             13, 8, 41,
                             14, 9, 43,
                             16, 9, 45,
                             17, 10, 48,
                             18, 10, 50,
                             20, 11, 52,
                             21, 11, 55,
                             22, 11, 57,
                             24, 12, 60,
                             25, 12, 62,
                             27, 12, 65,
                             28, 12, 67,
                             30, 12, 69,
                             31, 12, 72,
                             33, 12, 74,
                             35, 12, 76,
                             36, 12, 79,
                             38, 12, 81,
                             40, 11, 83,
                             41, 11, 85,
                             43, 11, 87,
                             45, 11, 89,
                             47, 10, 91,
                             49, 10, 92,
                             50, 10, 94,
                             52, 10, 95,
                             54, 9, 97,
                             56, 9, 98,
                             57, 9, 99,
                             59, 9, 100,
                             61, 9, 101,
                             62, 9, 102,
                             64, 10, 103,
                             66, 10, 104,
                             68, 10, 104,
                             69, 10, 105,
                             71, 11, 106,
                             73, 11, 106,
                             74, 12, 107,
                             76, 12, 107,
                             77, 13, 108,
                             79, 13, 108,
                             81, 14, 108,
                             82, 14, 109,
                             84, 15, 109,
                             85, 15, 109,
                             87, 16, 110,
                             89, 16, 110,
                             90, 17, 110,
                             92, 18, 110,
                             93, 18, 110,
                             95, 19, 110,
                             97, 19, 110,
                             98, 20, 110,
                             100, 21, 110,
                             101, 21, 110,
                             103, 22, 110,
                             105, 22, 110,
                             106, 23, 110,
                             108, 24, 110,
                             109, 24, 110,
                             111, 25, 110,
                             113, 25, 110,
                             114, 26, 110,
                             116, 26, 110,
                             117, 27, 110,
                             119, 28, 109,
                             120, 28, 109,
                             122, 29, 109,
                             124, 29, 109,
                             125, 30, 109,
                             127, 30, 108,
                             128, 31, 108,
                             130, 32, 108,
                             132, 32, 107,
                             133, 33, 107,
                             135, 33, 107,
                             136, 34, 106,
                             138, 34, 106,
                             140, 35, 105,
                             141, 35, 105,
                             143, 36, 105,
                             144, 37, 104,
                             146, 37, 104,
                             147, 38, 103,
                             149, 38, 103,
                             151, 39, 102,
                             152, 39, 102,
                             154, 40, 101,
                             155, 41, 100,
                             157, 41, 100,
                             159, 42, 99,
                             160, 42, 99,
                             162, 43, 98,
                             163, 44, 97,
                             165, 44, 96,
                             166, 45, 96,
                             168, 46, 95,
                             169, 46, 94,
                             171, 47, 94,
                             173, 48, 93,
                             174, 48, 92,
                             176, 49, 91,
                             177, 50, 90,
                             179, 50, 90,
                             180, 51, 89,
                             182, 52, 88,
                             183, 53, 87,
                             185, 53, 86,
                             186, 54, 85,
                             188, 55, 84,
                             189, 56, 83,
                             191, 57, 82,
                             192, 58, 81,
                             193, 58, 80,
                             195, 59, 79,
                             196, 60, 78,
                             198, 61, 77,
                             199, 62, 76,
                             200, 63, 75,
                             202, 64, 74,
                             203, 65, 73,
                             204, 66, 72,
                             206, 67, 71,
                             207, 68, 70,
                             208, 69, 69,
                             210, 70, 68,
                             211, 71, 67,
                             212, 72, 66,
                             213, 74, 65,
                             215, 75, 63,
                             216, 76, 62,
                             217, 77, 61,
                             218, 78, 60,
                             219, 80, 59,
                             221, 81, 58,
                             222, 82, 56,
                             223, 83, 55,
                             224, 85, 54,
                             225, 86, 53,
                             226, 87, 52,
                             227, 89, 51,
                             228, 90, 49,
                             229, 92, 48,
                             230, 93, 47,
                             231, 94, 46,
                             232, 96, 45,
                             233, 97, 43,
                             234, 99, 42,
                             235, 100, 41,
                             235, 102, 40,
                             236, 103, 38,
                             237, 105, 37,
                             238, 106, 36,
                             239, 108, 35,
                             239, 110, 33,
                             240, 111, 32,
                             241, 113, 31,
                             241, 115, 29,
                             242, 116, 28,
                             243, 118, 27,
                             243, 120, 25,
                             244, 121, 24,
                             245, 123, 23,
                             245, 125, 21,
                             246, 126, 20,
                             246, 128, 19,
                             247, 130, 18,
                             247, 132, 16,
                             248, 133, 15,
                             248, 135, 14,
                             248, 137, 12,
                             249, 139, 11,
                             249, 140, 10,
                             249, 142, 9,
                             250, 144, 8,
                             250, 146, 7,
                             250, 148, 7,
                             251, 150, 6,
                             251, 151, 6,
                             251, 153, 6,
                             251, 155, 6,
                             251, 157, 7,
                             252, 159, 7,
                             252, 161, 8,
                             252, 163, 9,
                             252, 165, 10,
                             252, 166, 12,
                             252, 168, 13,
                             252, 170, 15,
                             252, 172, 17,
                             252, 174, 18,
                             252, 176, 20,
                             252, 178, 22,
                             252, 180, 24,
                             251, 182, 26,
                             251, 184, 29,
                             251, 186, 31,
                             251, 188, 33,
                             251, 190, 35,
                             250, 192, 38,
                             250, 194, 40,
                             250, 196, 42,
                             250, 198, 45,
                             249, 199, 47,
                             249, 201, 50,
                             249, 203, 53,
                             248, 205, 55,
                             248, 207, 58,
                             247, 209, 61,
                             247, 211, 64,
                             246, 213, 67,
                             246, 215, 70,
                             245, 217, 73,
                             245, 219, 76,
                             244, 221, 79,
                             244, 223, 83,
                             244, 225, 86,
                             243, 227, 90,
                             243, 229, 93,
                             242, 230, 97,
                             242, 232, 101,
                             242, 234, 105,
                             241, 236, 109,
                             241, 237, 113,
                             241, 239, 117,
                             241, 241, 121,
                             242, 242, 125,
                             242, 244, 130,
                             243, 245, 134,
                             243, 246, 138,
                             244, 248, 142,
                             245, 249, 146,
                             246, 250, 150,
                             248, 251, 154,
                             249, 252, 157,
                             250, 253, 161,
                             252, 255, 164};

const double orbium0[20][20] = {
    {0, 0, 0, 0, 0, 0, 0.1, 0.14, 0.1, 0, 0, 0.03, 0.03, 0, 0, 0.3, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0.08, 0.24, 0.3, 0.3, 0.18, 0.14, 0.15, 0.16, 0.15, 0.09, 0.2, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0.15, 0.34, 0.44, 0.46, 0.38, 0.18, 0.14, 0.11, 0.13, 0.19, 0.18, 0.45, 0, 0, 0},
    {0, 0, 0, 0, 0.06, 0.13, 0.39, 0.5, 0.5, 0.37, 0.06, 0, 0, 0, 0.02, 0.16, 0.68, 0, 0, 0},
    {0, 0, 0, 0.11, 0.17, 0.17, 0.33, 0.4, 0.38, 0.28, 0.14, 0, 0, 0, 0, 0, 0.18, 0.42, 0, 0},
    {0, 0, 0.09, 0.18, 0.13, 0.06, 0.08, 0.26, 0.32, 0.32, 0.27, 0, 0, 0, 0, 0, 0, 0.82, 0, 0},
    {0.27, 0, 0.16, 0.12, 0, 0, 0, 0.25, 0.38, 0.44, 0.45, 0.34, 0, 0, 0, 0, 0, 0.22, 0.17, 0},
    {0, 0.07, 0.2, 0.02, 0, 0, 0, 0.31, 0.48, 0.57, 0.6, 0.57, 0, 0, 0, 0, 0, 0, 0.49, 0},
    {0, 0.59, 0.19, 0, 0, 0, 0, 0.2, 0.57, 0.69, 0.76, 0.76, 0.49, 0, 0, 0, 0, 0, 0.36, 0},
    {0, 0.58, 0.19, 0, 0, 0, 0, 0, 0.67, 0.83, 0.9, 0.92, 0.87, 0.12, 0, 0, 0, 0, 0.22, 0.07},
    {0, 0, 0.46, 0, 0, 0, 0, 0, 0.7, 0.93, 1, 1, 1, 0.61, 0, 0, 0, 0, 0.18, 0.11},
    {0, 0, 0.82, 0, 0, 0, 0, 0, 0.47, 1, 1, 0.98, 1, 0.96, 0.27, 0, 0, 0, 0.19, 0.1},
    {0, 0, 0.46, 0, 0, 0, 0, 0, 0.25, 1, 1, 0.84, 0.92, 0.97, 0.54, 0.14, 0.04, 0.1, 0.21, 0.05},
    {0, 0, 0, 0.4, 0, 0, 0, 0, 0.09, 0.8, 1, 0.82, 0.8, 0.85, 0.63, 0.31, 0.18, 0.19, 0.2, 0.01},
    {0, 0, 0, 0.36, 0.1, 0, 0, 0, 0.05, 0.54, 0.86, 0.79, 0.74, 0.72, 0.6, 0.39, 0.28, 0.24, 0.13, 0},
    {0, 0, 0, 0.01, 0.3, 0.07, 0, 0, 0.08, 0.36, 0.64, 0.7, 0.64, 0.6, 0.51, 0.39, 0.29, 0.19, 0.04, 0},
    {0, 0, 0, 0, 0.1, 0.24, 0.14, 0.1, 0.15, 0.29, 0.45, 0.53, 0.52, 0.46, 0.4, 0.31, 0.21, 0.08, 0, 0},
    {0, 0, 0, 0, 0, 0.08, 0.21, 0.21, 0.22, 0.29, 0.36, 0.39, 0.37, 0.33, 0.26, 0.18, 0.09, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.03, 0.13, 0.19, 0.22, 0.24, 0.24, 0.23, 0.18, 0.13, 0.05, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0.02, 0.06, 0.08, 0.09, 0.07, 0.05, 0.01, 0, 0, 0, 0, 0}};
const double orbium90[20][20] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.07, 0.11, 0.1, 0.05, 0.01, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.17, 0.49, 0.36, 0.22, 0.18, 0.19, 0.21, 0.2, 0.13, 0.04, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.42, 0.82, 0.22, 0, 0, 0, 0, 0, 0.1, 0.19, 0.24, 0.19, 0.08, 0, 0, 0},
    {0, 0, 0.45, 0.68, 0.18, 0, 0, 0, 0, 0, 0, 0, 0.04, 0.18, 0.28, 0.29, 0.21, 0.09, 0, 0},
    {0.3, 0.2, 0.18, 0.16, 0, 0, 0, 0, 0, 0, 0, 0, 0.14, 0.31, 0.39, 0.39, 0.31, 0.18, 0.05, 0},
    {0, 0.09, 0.19, 0.02, 0, 0, 0, 0, 0, 0, 0, 0.27, 0.54, 0.63, 0.6, 0.51, 0.4, 0.26, 0.13, 0.01},
    {0, 0.15, 0.13, 0, 0, 0, 0, 0, 0, 0.12, 0.61, 0.96, 0.97, 0.85, 0.72, 0.6, 0.46, 0.33, 0.18, 0.05},
    {0.03, 0.16, 0.11, 0, 0, 0, 0, 0, 0.49, 0.87, 1, 1, 0.92, 0.8, 0.74, 0.64, 0.52, 0.37, 0.23, 0.07},
    {0.03, 0.15, 0.14, 0, 0, 0, 0.34, 0.57, 0.76, 0.92, 1, 0.98, 0.84, 0.82, 0.79, 0.7, 0.53, 0.39, 0.24, 0.09},
    {0, 0.14, 0.18, 0.06, 0.14, 0.27, 0.45, 0.6, 0.76, 0.9, 1, 1, 1, 1, 0.86, 0.64, 0.45, 0.36, 0.24, 0.08},
    {0, 0.18, 0.38, 0.37, 0.28, 0.32, 0.44, 0.57, 0.69, 0.83, 0.93, 1, 1, 0.8, 0.54, 0.36, 0.29, 0.29, 0.22, 0.06},
    {0.1, 0.3, 0.46, 0.5, 0.38, 0.32, 0.38, 0.48, 0.57, 0.67, 0.7, 0.47, 0.25, 0.09, 0.05, 0.08, 0.15, 0.22, 0.19, 0.02},
    {0.14, 0.3, 0.44, 0.5, 0.4, 0.26, 0.25, 0.31, 0.2, 0, 0, 0, 0, 0, 0, 0, 0.1, 0.21, 0.13, 0},
    {0.1, 0.24, 0.34, 0.39, 0.33, 0.08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.14, 0.21, 0.03, 0},
    {0, 0.08, 0.15, 0.13, 0.17, 0.06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.07, 0.24, 0.08, 0, 0},
    {0, 0, 0, 0.06, 0.17, 0.13, 0, 0, 0, 0, 0, 0, 0, 0, 0.1, 0.3, 0.1, 0, 0, 0},
    {0, 0, 0, 0, 0.11, 0.18, 0.12, 0.02, 0, 0, 0, 0, 0, 0.4, 0.36, 0.01, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0.09, 0.16, 0.2, 0.19, 0.19, 0.46, 0.82, 0.46, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0.07, 0.59, 0.58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
const double orbium180[20][20] = {
    {0, 0, 0, 0, 0, 0.01, 0.05, 0.07, 0.09, 0.08, 0.06, 0.02, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.05, 0.13, 0.18, 0.23, 0.24, 0.24, 0.22, 0.19, 0.13, 0.03, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0.09, 0.18, 0.26, 0.33, 0.37, 0.39, 0.36, 0.29, 0.22, 0.21, 0.21, 0.08, 0, 0, 0, 0, 0},
    {0, 0, 0.08, 0.21, 0.31, 0.4, 0.46, 0.52, 0.53, 0.45, 0.29, 0.15, 0.1, 0.14, 0.24, 0.1, 0, 0, 0, 0},
    {0, 0.04, 0.19, 0.29, 0.39, 0.51, 0.6, 0.64, 0.7, 0.64, 0.36, 0.08, 0, 0, 0.07, 0.3, 0.01, 0, 0, 0},
    {0, 0.13, 0.24, 0.28, 0.39, 0.6, 0.72, 0.74, 0.79, 0.86, 0.54, 0.05, 0, 0, 0, 0.1, 0.36, 0, 0, 0},
    {0.01, 0.2, 0.19, 0.18, 0.31, 0.63, 0.85, 0.8, 0.82, 1, 0.8, 0.09, 0, 0, 0, 0, 0.4, 0, 0, 0},
    {0.05, 0.21, 0.1, 0.04, 0.14, 0.54, 0.97, 0.92, 0.84, 1, 1, 0.25, 0, 0, 0, 0, 0, 0.46, 0, 0},
    {0.1, 0.19, 0, 0, 0, 0.27, 0.96, 1, 0.98, 1, 1, 0.47, 0, 0, 0, 0, 0, 0.82, 0, 0},
    {0.11, 0.18, 0, 0, 0, 0, 0.61, 1, 1, 1, 0.93, 0.7, 0, 0, 0, 0, 0, 0.46, 0, 0},
    {0.07, 0.22, 0, 0, 0, 0, 0.12, 0.87, 0.92, 0.9, 0.83, 0.67, 0, 0, 0, 0, 0, 0.19, 0.58, 0},
    {0, 0.36, 0, 0, 0, 0, 0, 0.49, 0.76, 0.76, 0.69, 0.57, 0.2, 0, 0, 0, 0, 0.19, 0.59, 0},
    {0, 0.49, 0, 0, 0, 0, 0, 0, 0.57, 0.6, 0.57, 0.48, 0.31, 0, 0, 0, 0.02, 0.2, 0.07, 0},
    {0, 0.17, 0.22, 0, 0, 0, 0, 0, 0.34, 0.45, 0.44, 0.38, 0.25, 0, 0, 0, 0.12, 0.16, 0, 0.27},
    {0, 0, 0.82, 0, 0, 0, 0, 0, 0, 0.27, 0.32, 0.32, 0.26, 0.08, 0.06, 0.13, 0.18, 0.09, 0, 0},
    {0, 0, 0.42, 0.18, 0, 0, 0, 0, 0, 0.14, 0.28, 0.38, 0.4, 0.33, 0.17, 0.17, 0.11, 0, 0, 0},
    {0, 0, 0, 0.68, 0.16, 0.02, 0, 0, 0, 0.06, 0.37, 0.5, 0.5, 0.39, 0.13, 0.06, 0, 0, 0, 0},
    {0, 0, 0, 0.45, 0.18, 0.19, 0.13, 0.11, 0.14, 0.18, 0.38, 0.46, 0.44, 0.34, 0.15, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.2, 0.09, 0.15, 0.16, 0.15, 0.14, 0.18, 0.3, 0.3, 0.24, 0.08, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.3, 0, 0, 0.03, 0.03, 0, 0, 0.1, 0.14, 0.1, 0, 0, 0, 0, 0, 0}};
const double orbium270[20][20] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.27, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.58, 0.59, 0.07, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0.46, 0.82, 0.46, 0.19, 0.19, 0.2, 0.16, 0.09, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.01, 0.36, 0.4, 0, 0, 0, 0, 0, 0.02, 0.12, 0.18, 0.11, 0, 0, 0, 0},
    {0, 0, 0, 0.1, 0.3, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0.13, 0.17, 0.06, 0, 0, 0},
    {0, 0, 0.08, 0.24, 0.07, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.06, 0.17, 0.13, 0.15, 0.08, 0},
    {0, 0.03, 0.21, 0.14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.08, 0.33, 0.39, 0.34, 0.24, 0.1},
    {0, 0.13, 0.21, 0.1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.31, 0.25, 0.26, 0.4, 0.5, 0.44, 0.3, 0.14},
    {0.02, 0.19, 0.22, 0.15, 0.08, 0.05, 0.09, 0.25, 0.47, 0.7, 0.67, 0.57, 0.48, 0.38, 0.32, 0.38, 0.5, 0.46, 0.3, 0.1},
    {0.06, 0.22, 0.29, 0.29, 0.36, 0.54, 0.8, 1, 1, 0.93, 0.83, 0.69, 0.57, 0.44, 0.32, 0.28, 0.37, 0.38, 0.18, 0},
    {0.08, 0.24, 0.36, 0.45, 0.64, 0.86, 1, 1, 1, 1, 0.9, 0.76, 0.6, 0.45, 0.27, 0.14, 0.06, 0.18, 0.14, 0},
    {0.09, 0.24, 0.39, 0.53, 0.7, 0.79, 0.82, 0.84, 0.98, 1, 0.92, 0.76, 0.57, 0.34, 0, 0, 0, 0.14, 0.15, 0.03},
    {0.07, 0.23, 0.37, 0.52, 0.64, 0.74, 0.8, 0.92, 1, 1, 0.87, 0.49, 0, 0, 0, 0, 0, 0.11, 0.16, 0.03},
    {0.05, 0.18, 0.33, 0.46, 0.6, 0.72, 0.85, 0.97, 0.96, 0.61, 0.12, 0, 0, 0, 0, 0, 0, 0.13, 0.15, 0},
    {0.01, 0.13, 0.26, 0.4, 0.51, 0.6, 0.63, 0.54, 0.27, 0, 0, 0, 0, 0, 0, 0, 0.02, 0.19, 0.09, 0},
    {0, 0.05, 0.18, 0.31, 0.39, 0.39, 0.31, 0.14, 0, 0, 0, 0, 0, 0, 0, 0, 0.16, 0.18, 0.2, 0.3},
    {0, 0, 0.09, 0.21, 0.29, 0.28, 0.18, 0.04, 0, 0, 0, 0, 0, 0, 0, 0.18, 0.68, 0.45, 0, 0},
    {0, 0, 0, 0.08, 0.19, 0.24, 0.19, 0.1, 0, 0, 0, 0, 0, 0.22, 0.82, 0.42, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.04, 0.13, 0.2, 0.21, 0.19, 0.18, 0.22, 0.36, 0.49, 0.17, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.01, 0.05, 0.1, 0.11, 0.07, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// Function to place orbiums into the wild
double *place_orbium(double *world, unsigned int rows, unsigned int cols, unsigned int y, unsigned int x, unsigned int angle)
{
    double const(*orbium)[ORBIUM_SIZE];
    if (world != NULL)
    {
        switch (angle)
        {
        case 0:
            orbium = orbium0;
            break;
        case 90:
            orbium = orbium90;
            break;
        case 180:
            orbium = orbium180;
            break;
        case 270:
            orbium = orbium270;
            break;
        default:
            return world;
        }
        for (int i = 0; i < ORBIUM_SIZE; i++)
        {
            for (int j = 0; j < ORBIUM_SIZE; j++)
            {
                world[(y + i) % rows * cols + (x + j) % cols] = orbium[i][j];
            }
        }
    }
    return world;
}