
#include "SDL2/SDL.h"
#include "jpeglib.h"
#include <png.h>

#include "Image.h"

Image* loadJPEG(const char* filename) {
    struct jpeg_decompress_struct cinfo;

    struct jpeg_error_mgr jerr;
    FILE* file;

    if((file = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Cannot open JPEG %s\n", filename);
        return NULL;
    }

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);

    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    int rowWidth = cinfo.output_width*cinfo.output_components;
    unsigned char** buffer = (unsigned char**)malloc(sizeof(unsigned char*));
    buffer[0] = (unsigned char*)malloc(sizeof(unsigned char)*rowWidth);

    Image* output = (Image*)malloc(sizeof(Image));
    output->w = cinfo.output_width;
    output->h = cinfo.output_height;
    output->numComponents = cinfo.output_components;
    output->data = (unsigned char*)malloc(sizeof(unsigned char)*rowWidth*cinfo.output_height);

    printf("Image info:\n");
    printf("-Width: %d\n", cinfo.output_width);
    printf("-Height: %d\n", cinfo.output_height);
    printf("-Number of components: %d\n", cinfo.output_components);
    
    while(cinfo.output_scanline < cinfo.output_height) {
        int row = cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(output->data+(row*rowWidth), buffer[0], rowWidth);
    }
    jpeg_finish_decompress(&cinfo);
    fclose(file);
    free(buffer);

    return output;
}

Image* loadPNG(const char* filename) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    Image* output = NULL;
    if(png_image_begin_read_from_file(&image, filename) != 0) {
        png_bytep buffer;
        image.format = PNG_FORMAT_RGB;
        buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
        if(buffer != NULL &&
           png_image_finish_read(&image, 0, buffer, 0, NULL) != 0) {
            int rowStride = PNG_IMAGE_ROW_STRIDE(image);
            int imageSize = PNG_IMAGE_SIZE(image);
            int componentSize = PNG_IMAGE_SAMPLE_COMPONENT_SIZE(PNG_FORMAT_RGB);
            int numComponents = PNG_IMAGE_SAMPLE_CHANNELS(PNG_FORMAT_RGB);
            if(PNG_IMAGE_SAMPLE_COMPONENT_SIZE(PNG_FORMAT_RGB) != 1) {
                printf("Component size is not 1 but assumed to be 1!\n");
            }
            output = (Image*)malloc(sizeof(image));
            output->w = rowStride/(numComponents*componentSize);
            output->h = PNG_IMAGE_SIZE(image)/PNG_IMAGE_ROW_STRIDE(image);
            output->numComponents = numComponents;
            output->data = (unsigned char*)malloc(imageSize);
            memcpy(output->data, buffer, imageSize);
        } else {
            if(buffer == NULL) {
                printf("Could not allocate enough memory to read PNG\n");
                png_image_free(&image);
            } else {
                printf("Could not finish reading PNG after header\n");
            }
        }
        if(buffer != NULL)
            free(buffer);
    } else {
        printf("Could not read PNG header/file\n");
    }
    
    return output;
}

Image* createCopy(Image* image) {
    if(image == NULL) {
        return NULL;
    }
    Image* output = (Image*)malloc(sizeof(Image));
    output->w = image->w;
    output->h = image->h;
    output->numComponents = image->numComponents;
    if(image->data == NULL) {
        output->data = image->data;
    } else {
        int size = output->w*output->h*output->numComponents;
        output->data = (unsigned char*)malloc(size*sizeof(unsigned char));
        memcpy(output->data, image->data, size);
    }
    return output;
}

int grayToRGB(Image* image) {
    if(image == NULL) {
        return 1;
    }
    if(image->numComponents != 1) {
        return 1;
    }

    int w = image->w;
    int h = image->h;
    unsigned char* src = image->data;
    unsigned char* newData = (unsigned char*)malloc(sizeof(unsigned char)*(w*h*3));

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            unsigned char pixel = src[y*w + x];
            int loc = y*(3*w) + 3*x;
            newData[loc] = pixel;
            newData[loc+1] = pixel;
            newData[loc+2] = pixel;
        }
    }

    free(image->data);
    image->data = newData;
    image->numComponents = 3;
    return 0;
}

int RGBAToRGB(Image* image) {
    if(image == NULL) {
        return 1;
    }
    if(image->numComponents != 4) {
        return 1;
    }
    int w = image->w;
    int h = image->h;
    unsigned char* src = image->data;
    unsigned char* dst = (unsigned char*)malloc(sizeof(unsigned char)*(w*h*3));
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            int srcLoc = 4*(y*w+x);
            int dstLoc = 3*(y*w+x);
            dst[dstLoc+0] = src[srcLoc+0];
            dst[dstLoc+1] = src[srcLoc+1];
            dst[dstLoc+2] = src[srcLoc+2];
        }
    }
    free(image->data);
    image->data = dst;
    image->numComponents = 3;
    return 0;
}

int endsWith(const char* string1, const char* string2) {
    if(string1 == NULL || string2 == NULL)
        return 0;
    int s1length = strlen(string1);
    int s2length = strlen(string2);
    if(s1length < s2length)
        return 0;
    return strncmp(string1 + (s1length - s2length), string2, s2length) == 0;
}

Image* loadImage(const char* filename) {
    Image* image = NULL;
    if(endsWith(filename,".png")) {
    	printf("Loading PNG\n");
        image = loadPNG(filename);
    } else if(endsWith(filename,".jpg")) {
    	printf("Loading JPG\n");
    	image = loadJPEG(filename);
    } else if(endsWith(filename,".bmp")) {
        SDL_Surface* surface = SDL_LoadBMP(filename);
        image = convertToImage(surface);
        SDL_FreeSurface(surface);
    }
    if(image == NULL) {
        printf("Could not load image: Unsupported file extension\n");
        return NULL;
    }
    if(image->numComponents == 1) {
        grayToRGB(image);
    }
    if(image->numComponents == 4) {
        RGBAToRGB(image);
    }
    return image;
}

Image* convertToImage(SDL_Surface* surface) {
    if(surface == NULL) {
        printf("Could not convert surface to image: surface is NULL\n");
        return NULL;
    }
    printf("Format: r - %x\tg - %x\tb - %x\n",
           surface->format->Rmask,
           surface->format->Gmask,
           surface->format->Bmask);
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_BGR888, 0);
    SDL_LockSurface(surface);
    Image* image = (Image*)malloc(sizeof(Image));
    image->w = surface->w;
    image->h = surface->h;
    image->numComponents = surface->format->BytesPerPixel;
    
    int comp = image->numComponents;
    int w = image->w;
    int h = image->h;
    int pitch = surface->pitch;
    image->data = (unsigned char*)malloc(sizeof(unsigned char)*w*h*comp);
    memcpy(image->data, surface->pixels, w*h*comp);
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);
    return image;
}

SDL_Surface* convertToSurface(Image* image) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);
    Uint32* pixels = (Uint32*)surface->pixels;
    for(int y = 0; y < image->h; y++) {
        for(int x = 0; x < image->w; x++) {
            int dataLoc = image->numComponents*(y*image->w + x);
            int r,g,b;
            if(image->numComponents == 1) {
                r = image->data[dataLoc];
                g = r;
                b = r;
            } else {
                r = image->data[dataLoc+0];
                g = image->data[dataLoc+1];
                b = image->data[dataLoc+2];
            }
            pixels[y*image->w + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }
    SDL_UnlockSurface(surface);
    return surface;
}

SDL_Surface* loadSurface(const char* filename) {
    Image* image = loadImage(filename);
    SDL_Surface* surface = convertToSurface(image);
    free(image->data);
    free(image);
    return surface;
}
