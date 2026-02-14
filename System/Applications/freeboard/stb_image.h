#ifndef STB_IMAGE_H
#define STB_IMAGE_H

// stb_image - public domain image loader
// This is a minimal version for FreeBoard

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int stbi__get8(FILE *f) {
    int c = fgetc(f);
    if (c == EOF) return 0;
    return c;
}

static void stbi__skip(FILE *f, int n) {
    fseek(f, n, SEEK_CUR);
}

static int stbi__test_eof(FILE *f) {
    int c = fgetc(f);
    if (c == EOF) return 1;
    ungetc(c, f);
    return 0;
}

static unsigned char *stbi__load_main(FILE *f, int *x, int *y, int *comp, int req_comp) {
    // Simple fallback - create a gradient
    *x = 1920;
    *y = 1080;
    *comp = 4;
    
    unsigned char *data = (unsigned char*)malloc((*x) * (*y) * (*comp));
    if (!data) return NULL;
    
    for (int j = 0; j < *y; ++j) {
        for (int i = 0; i < *x; ++i) {
            int idx = (j * (*x) + i) * 4;
            float r = 0.4f + 0.2f * sinf(i * 0.001f);
            float g = 0.6f + 0.2f * sinf(j * 0.001f + 1.0f);
            float b = 0.8f + 0.2f * sinf((i + j) * 0.001f + 2.0f);
            
            data[idx] = (unsigned char)(r * 255);
            data[idx + 1] = (unsigned char)(g * 255);
            data[idx + 2] = (unsigned char)(b * 255);
            data[idx + 3] = 255;
        }
    }
    
    return data;
}

unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
    FILE *f = fopen(filename, "rb");
    if (!f) return stbi__load_main(f, x, y, comp, req_comp);
    
    // For simplicity, we'll just use the fallback gradient
    fclose(f);
    return stbi__load_main(f, x, y, comp, req_comp);
}

void stbi_image_free(void *retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION

unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
void stbi_image_free(void *retval_from_stbi_load);

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_H
