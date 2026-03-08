
// OPENMP GRAYSCALE FILTER


#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>        /* OpenMP header – gives us omp_get_wtime() etc. */

int main() {
    /* ---------- 1. Load the input image ---------- */
    int width, height, channels;
    unsigned char *img = stbi_load("../images/testing_images/input.jpg",
                                   &width, &height, &channels, 3);
    if (!img) {
        printf("ERROR: Could not load image.\n");
        return 1;
    }
    printf("Image loaded: %d x %d pixels\n", width, height);

    /* ---------- 2. Allocate output ---------- */
    unsigned char *output = (unsigned char *)malloc(width * height);
    if (!output) {
        printf("ERROR: Memory allocation failed.\n");
        stbi_image_free(img);
        return 1;
    }

    /* ---------- 3. Print thread info ---------- */
    int num_threads;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }
    printf("Running with %d OpenMP threads\n", num_threads);

    /* ---------- 4. Start timing (OpenMP wall-clock timer) ---------- */
    double start = omp_get_wtime();


    #pragma omp parallel for schedule(static)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;

            unsigned char r = img[idx + 0];
            unsigned char g = img[idx + 1];
            unsigned char b = img[idx + 2];

            /* Same formula as serial – just running on many cores */
            output[y * width + x] = (unsigned char)(0.299f * r +
                                                     0.587f * g +
                                                     0.114f * b);
        }
    }

    /* ---------- 6. Stop timing ---------- */
    double elapsed = omp_get_wtime() - start;
    printf("[OPENMP] Grayscale time: %.4f seconds\n", elapsed);

    /* ---------- 7. Save output ---------- */
    stbi_write_jpg("output/grayscale/gray_openmp.jpg", width, height, 1, output, 95);
    printf("Output saved to: output/grayscale/gray_openmp.jpg\n");

    stbi_image_free(img);
    free(output);

    return 0;
}
