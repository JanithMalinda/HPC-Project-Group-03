

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ---- 5x5 Gaussian kernel weights ---- */
static const int KERNEL[5][5] = {
    { 2,  4,  5,  4,  2},
    { 4,  9, 12,  9,  4},
    { 5, 12, 15, 12,  5},
    { 4,  9, 12,  9,  4},
    { 2,  4,  5,  4,  2}
};
static const int KERNEL_SUM = 159;  /* sum of all weights */
static const int RADIUS = 2;        /* kernel goes 2 pixels in each direction */

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

    /* ---------- 2. Allocate output (same size, 3 channels) ---------- */
    unsigned char *output = (unsigned char *)malloc(width * height * 3);
    if (!output) {
        printf("ERROR: Memory allocation failed.\n");
        stbi_image_free(img);
        return 1;
    }

    /* ---------- 3. Start timing ---------- */
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* ---------- 4. SERIAL Gaussian blur ---------- */
    /*
     * For each pixel (x, y):
     *   - Look at its 5x5 neighborhood (ky from -2 to +2, kx from -2 to +2)
     *   - Multiply each neighbor's color by the kernel weight
     *   - Sum all products, divide by KERNEL_SUM → new pixel value
     *
     * BORDER HANDLING (clamp):
     *   ny = y + ky might go outside the image.
     *   We clamp it: keep it between 0 and height-1.
     *   This avoids reading invalid memory.
     */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int sum_r = 0, sum_g = 0, sum_b = 0;

            /* Slide the 5x5 kernel over neighbors */
            for (int ky = -RADIUS; ky <= RADIUS; ky++) {
                for (int kx = -RADIUS; kx <= RADIUS; kx++) {

                    /* Clamp neighbor coordinates to stay inside image */
                    int ny = y + ky;
                    int nx = x + kx;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;
                    if (nx < 0) nx = 0;
                    if (nx >= width)  nx = width  - 1;

                    int n_idx = (ny * width + nx) * 3;
                    int weight = KERNEL[ky + RADIUS][kx + RADIUS];

                    sum_r += img[n_idx + 0] * weight;
                    sum_g += img[n_idx + 1] * weight;
                    sum_b += img[n_idx + 2] * weight;
                }
            }

            /* Normalize and store result */
            int out_idx = (y * width + x) * 3;
            output[out_idx + 0] = (unsigned char)(sum_r / KERNEL_SUM);
            output[out_idx + 1] = (unsigned char)(sum_g / KERNEL_SUM);
            output[out_idx + 2] = (unsigned char)(sum_b / KERNEL_SUM);
        }
    }

    /* ---------- 5. Stop timing ---------- */
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("[SERIAL] Gaussian Blur time: %.4f seconds\n", elapsed);

    /* ---------- 6. Save output ---------- */
    stbi_write_jpg("output/gaussian/gaussian_serial.jpg", width, height, 3, output, 95);
    printf("Output saved to: output/gaussian/gaussian_serial.jpg\n");

    stbi_image_free(img);
    free(output);
    return 0;
}
