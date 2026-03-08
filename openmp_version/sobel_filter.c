
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

static const int Gx[3][3] = {{-1,0,+1},{-2,0,+2},{-1,0,+1}};
static const int Gy[3][3] = {{-1,-2,-1},{0,0,0},{+1,+2,+1}};

int main() {
    int width, height, channels;
    unsigned char *img = stbi_load("../images/testing_images/input.jpg",
                                   &width, &height, &channels, 3);
    if (!img) { printf("ERROR: Could not load image.\n"); return 1; }
    printf("Image loaded: %d x %d pixels\n", width, height);

    int num_threads;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }
    printf("Running with %d OpenMP threads\n", num_threads);

    unsigned char *gray   = (unsigned char *)malloc(width * height);
    unsigned char *output = (unsigned char *)malloc(width * height);

    double start = omp_get_wtime();

    /* Step 1: Parallel grayscale conversion */
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < width * height; i++) {
        gray[i] = (unsigned char)(0.299f * img[i*3+0] +
                                  0.587f * img[i*3+1] +
                                  0.114f * img[i*3+2]);
    }

    /* Step 2: Parallel Sobel */
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int gx_sum = 0, gy_sum = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int ny = y + ky; int nx = x + kx;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;
                    if (nx < 0) nx = 0;
                    if (nx >= width)  nx = width  - 1;

                    unsigned char pixel = gray[ny * width + nx];
                    gx_sum += pixel * Gx[ky+1][kx+1];
                    gy_sum += pixel * Gy[ky+1][kx+1];
                }
            }

            int magnitude = (int)sqrtf((float)(gx_sum*gx_sum + gy_sum*gy_sum));
            if (magnitude > 255) magnitude = 255;
            if (magnitude < 0)   magnitude = 0;
            output[y * width + x] = (unsigned char)magnitude;
        }
    }

    double elapsed = omp_get_wtime() - start;
    printf("[OPENMP] Sobel Edge Detection time: %.4f seconds\n", elapsed);

    stbi_write_jpg("output/sobel/sobel_openmp.jpg", width, height, 1, output, 95);
    printf("Output saved to: output/sobel/sobel_openmp.jpg\n");

    stbi_image_free(img);
    free(gray);
    free(output);
    return 0;
}
