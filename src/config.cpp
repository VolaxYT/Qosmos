#include "config.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<float> normalize_rgb(int red, int green, int blue){
    std::vector<float> ncolor;
    ncolor.resize(3);
    ncolor[0] = (red - 127.5f)/127.5f;
    ncolor[1] = (green - 127.5f)/127.5f;
    ncolor[2] = (blue - 127.5f)/127.5f;
    return ncolor;
}