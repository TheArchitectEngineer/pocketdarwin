/*
 * Fallback stb_image wrapper when vendor/stb is unavailable.
 * Decoding is intentionally disabled; callers receive NULL and a reason string.
 */

#include <stddef.h>

static const char *touchhle_stbi_reason = "stb_image vendor sources missing";

void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert) {
    (void)flag_true_if_should_convert;
}

void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply) {
    (void)flag_true_if_should_unpremultiply;
}

unsigned char *stbi_load_from_memory(const unsigned char *buffer, int len, int *x, int *y,
                                     int *channels_in_file, int desired_channels) {
    (void)buffer;
    (void)len;
    (void)x;
    (void)y;
    (void)channels_in_file;
    (void)desired_channels;
    return NULL;
}

void stbi_image_free(void *retval_from_stbi_load) {
    (void)retval_from_stbi_load;
}

const char *stbi_failure_reason(void) {
    return touchhle_stbi_reason;
}
