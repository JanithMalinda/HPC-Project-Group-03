

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    /* ---------- 1. Load the input image ---------- */
    int width, height, channels;
    unsigned char *img = stbi_load("../images/testing_images/input.jpg",
                                   &width, &height, &channels, 3);
    if (!img) {
        printf("ERROR: Could not load image.\n");
        printf("Make sure 'input.jpg' is inside images/testing_images/\n");
        return 1;
    }
    printf("Image loaded: %d x %d pixels, %d channels\n", width, height, channels);

    /* ---------- 2. Allocate output (grayscale = 1 channel) ---------- */
    unsigned char *output = (unsigned char *)malloc(width * height);
    if (!output) {
        printf("ERROR: Memory allocation failed.\n");
        stbi_image_free(img);
        return 1;
    }

    /* ---------- 3. Start timing ---------- */
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);


    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;         /* index into RGB array */

            unsigned char r = img[idx + 0];         /* Red   channel */
            unsigned char g = img[idx + 1];         /* Green channel */
            unsigned char b = img[idx + 2];         /* Blue  channel */

            /* Luminance formula (ITU-R BT.601) */
            unsigned char gray = (unsigned char)(0.299f * r +
                                                 0.587f * g +
                                                 0.114f * b);

            output[y * width + x] = gray;           /* store result */
        }
    }

    /* ---------- 5. Stop timing ---------- */
    timespec_get(&end, TIME_UTC);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("[SERIAL] Grayscale time: %.4f seconds\n", elapsed);

    /* ---------- 6. Save output image ---------- */
    stbi_write_jpg("output/grayscale/gray_serial.jpg", width, height, 1, output, 95);
    printf("Output saved to: output/grayscale/gray_serial.jpg\n");

    /* ---------- 7. Free memory ---------- */
    stbi_image_free(img);
    free(output);

    return 0;
}
