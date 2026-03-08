
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* Laplacian kernel */
static const int LAPLACIAN[3][3] = {
    { 0, -1,  0},
    {-1, +4, -1},
    { 0, -1,  0}
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

    /* ---------- 2. Convert to grayscale ---------- */
    unsigned char *gray = (unsigned char *)malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        gray[i] = (unsigned char)(0.299f * img[i*3+0] +
                                  0.587f * img[i*3+1] +
                                  0.114f * img[i*3+2]);
    }

    unsigned char *output = (unsigned char *)malloc(width * height);

    /* ---------- 3. Start timing ---------- */
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* ---------- 4. SERIAL Laplacian filter ---------- */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int sum = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int ny = y + ky;
                    int nx = x + kx;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;
                    if (nx < 0) nx = 0;
                    if (nx >= width)  nx = width - 1;

                    sum += gray[ny * width + nx] * LAPLACIAN[ky + 1][kx + 1];
                }
            }

            /* Take absolute value (edges can be negative or positive) */
            if (sum < 0) sum = -sum;
            if (sum > 255) sum = 255;

            output[y * width + x] = (unsigned char)sum;
        }
    }

    /* ---------- 5. Stop timing ---------- */
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("[SERIAL] Laplacian Filter time: %.4f seconds\n", elapsed);

    /* ---------- 6. Save output ---------- */
    stbi_write_jpg("output/laplacian/laplacian_serial.jpg", width, height, 1, output, 95);
    printf("Output saved to: output/laplacian/laplacian_serial.jpg\n");

    stbi_image_free(img);
    free(gray);
    free(output);
    return 0;
}
