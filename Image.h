
#ifndef IMAGE_H
#define IMAGE_H

#include "SDL2/SDL.h"

typedef struct {
    int w;
    int h;
    int numComponents;
    unsigned char* data;
} Image;

Image* loadJPEG(const char* filename);
Image* loadPNG(const char* filename);
Image* createCopy(Image* image);
int grayToRGB(Image* image);
int RGBAToRGB(Image* image);
Image* loadImage(const char* filename);
Image* convertToImage(SDL_Surface* surface);
SDL_Surface* convertToSurface(Image* image);
SDL_Surface* loadSurface(const char* filename);

#endif
