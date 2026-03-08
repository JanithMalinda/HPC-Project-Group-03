/*
 * ============================================================
 *  SERIAL SOBEL EDGE DETECTION FILTER
 *  Member 3 – EG/2020/4064 – Malinda H. W. J.
 *  Group 03 | EC7207 HPC Project
 * ============================================================
 *
 *  WHAT IS SOBEL EDGE DETECTION?
 *  It finds edges (sharp boundaries) in an image.
 *  An "edge" is a place where pixel brightness changes quickly.
 *
 *  HOW DOES IT WORK?
 *  It uses two 3x3 kernels to measure how fast brightness
 *  changes in the X direction (horizontal) and Y direction (vertical).
 *
 *  Gx kernel (detects vertical edges):      Gy kernel (detects horizontal edges):
 *  -1  0  +1                                -1 -2 -1
 *  -2  0  +2                                  0  0  0
 *  -1  0  +1                                +1 +2 +1
 *
 *  For each pixel:
 *    1. Convert to grayscale first (edges are easier to find on gray)
 *    2. Apply Gx kernel → how much change in X direction
 *    3. Apply Gy kernel → how much change in Y direction
 *    4. Magnitude = sqrt(Gx^2 + Gy^2)  → overall edge strength
 *    5. Clamp to [0, 255]
 *
 *  High magnitude = strong edge (bright in output)
 *  Low magnitude  = flat region (dark in output)
 * ============================================================
 */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* Sobel kernels */
static const int Gx[3][3] = {
    {-1, 0, +1},
    {-2, 0, +2},
    {-1, 0, +1}
};
static const int Gy[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    {+1, +2, +1}
};

int main() {
    /* ---------- 1. Load image ---------- */
    int width, height, channels;
    unsigned char *img = stbi_load("../images/testing_images/input.jpg",
                                   &width, &height, &channels, 3);
    if (!img) {
        printf("ERROR: Could not load image.\n");
        return 1;
    }
    printf("Image loaded: %d x %d pixels\n", width, height);

    /* ---------- 2. Convert to grayscale first ---------- */
    /*
     * Sobel works on single-channel (grayscale) images.
     * We first reduce the 3-channel RGB to 1-channel gray.
     */
    unsigned char *gray = (unsigned char *)malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        unsigned char r = img[i * 3 + 0];
        unsigned char g = img[i * 3 + 1];
        unsigned char b = img[i * 3 + 2];
        gray[i] = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
    }

    /* ---------- 3. Allocate output ---------- */
    unsigned char *output = (unsigned char *)malloc(width * height);

    /* ---------- 4. Start timing ---------- */
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* ---------- 5. SERIAL Sobel edge detection ---------- */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int gx_sum = 0, gy_sum = 0;

            /* Apply 3x3 Gx and Gy kernels */
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    /* Clamp to image borders */
                    int ny = y + ky;
                    int nx = x + kx;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;
                    if (nx < 0) nx = 0;
                    if (nx >= width)  nx = width  - 1;

                    unsigned char pixel = gray[ny * width + nx];

                    gx_sum += pixel * Gx[ky + 1][kx + 1];
                    gy_sum += pixel * Gy[ky + 1][kx + 1];
                }
            }

            /* Calculate edge magnitude */
            int magnitude = (int)sqrtf((float)(gx_sum * gx_sum + gy_sum * gy_sum));

            /* Clamp to 0–255 */
            if (magnitude > 255) magnitude = 255;
            if (magnitude < 0)   magnitude = 0;

            output[y * width + x] = (unsigned char)magnitude;
        }
    }

    /* ---------- 6. Stop timing ---------- */
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("[SERIAL] Sobel Edge Detection time: %.4f seconds\n", elapsed);

    /* ---------- 7. Save output ---------- */
    stbi_write_jpg("output/sobel/sobel_serial.jpg", width, height, 1, output, 95);
    printf("Output saved to: output/sobel/sobel_serial.jpg\n");

    stbi_image_free(img);
    free(gray);
    free(output);
    return 0;
}
