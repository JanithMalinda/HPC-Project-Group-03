/*
 *
 *  CHANGES FROM SERIAL:
 *  → Added  #pragma omp parallel for  before outer loop
 *  → Used  omp_get_wtime()  for accurate timing
 *  → The inner kernel loop is NOT parallelized on purpose –
 *    parallelizing the outer (row) loop is enough and avoids
 *    overhead from creating too many thread teams.
 *
 */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* ---- 5x5 Gaussian kernel (same as serial) ---- */
static const int KERNEL[5][5] = {
    { 2,  4,  5,  4,  2},
    { 4,  9, 12,  9,  4},
    { 5, 12, 15, 12,  5},
    { 4,  9, 12,  9,  4},
    { 2,  4,  5,  4,  2}
};
static const int KERNEL_SUM = 159;
static const int RADIUS = 2;

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

    unsigned char *output = (unsigned char *)malloc(width * height * 3);
    if (!output) {
        printf("ERROR: Memory allocation failed.\n");
        stbi_image_free(img);
        return 1;
    }

    /* ---------- 2. Print thread count ---------- */
    int num_threads;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }
    printf("Running with %d OpenMP threads\n", num_threads);

    /* ---------- 3. Start timing ---------- */
    double start = omp_get_wtime();

    /* ---------- 4. PARALLEL Gaussian blur ---------- */
    /*
     * The outer loop (y = rows) is parallelized.
     * Each thread handles a block of rows independently.
     *
     * IMPORTANT: Each pixel's output depends only on INPUT pixels
     * (we read from 'img', write to 'output').
     * No two threads write to the same output location.
     * → NO race condition → safe to parallelize.
     *
     * schedule(dynamic, 4):
     *   Rows are assigned in chunks of 4 dynamically.
     *   Useful if some rows take slightly longer (e.g., border rows).
     */
    #pragma omp parallel for schedule(dynamic, 4)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int sum_r = 0, sum_g = 0, sum_b = 0;

            for (int ky = -RADIUS; ky <= RADIUS; ky++) {
                for (int kx = -RADIUS; kx <= RADIUS; kx++) {

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

            int out_idx = (y * width + x) * 3;
            output[out_idx + 0] = (unsigned char)(sum_r / KERNEL_SUM);
            output[out_idx + 1] = (unsigned char)(sum_g / KERNEL_SUM);
            output[out_idx + 2] = (unsigned char)(sum_b / KERNEL_SUM);
        }
    }

    /* ---------- 5. Stop timing ---------- */
    double elapsed = omp_get_wtime() - start;
    printf("[OPENMP] Gaussian Blur time: %.4f seconds\n", elapsed);

    /* ---------- 6. Save output ---------- */
    stbi_write_jpg("output/gaussian/gaussian_openmp.jpg", width, height, 3, output, 95);
    printf("Output saved to: output/gaussian/gaussian_openmp.jpg\n");

    stbi_image_free(img);
    free(output);
    return 0;
}
