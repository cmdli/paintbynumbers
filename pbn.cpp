
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "SDL2/SDL.h"
#include "jpeglib.h"
#include <png.h>

#include "Point.h"
#include "Image.h"

inline double randDouble() {
    return ((double)rand())/RAND_MAX;
}

Point* createRandomPoint() {
    Point* point = new Point();
    point->x = randDouble();
    point->y = randDouble();
    point->r = randDouble();
    point->g = randDouble();
    point->b = randDouble();
    return point;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    if(argc < 2) {
        printf("Needs input argument.\n");
        return 1;
    }
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Could not init SDL\n");
        return 1;
    }

    Image* image = loadImage(argv[1]);
    if(image == NULL) {
        printf("Could not load image.\n");
        return 1;
    }
    if(image->numComponents != 3) {
        printf("Image isn't RGB, instead has %d components\n", image->numComponents);
        return 1;
    }

    double xweight = 1.0;
    double yweight = 1.0;
    
    printf("Converting to points...\n");
    std::vector<Point*> points;
    int pixelLoc,r,g,b;
    for(int y = 0; y < image->h; y++) {
        for(int x = 0; x < image->w; x++) {
            pixelLoc = 3*(y*image->w + x);
            double xp = (double)x*xweight;
            double yp = (double)y*yweight;
            double rp = image->data[pixelLoc];
            double gp = image->data[pixelLoc+1];
            double bp = image->data[pixelLoc+2];
            points.push_back(new Point(xp,yp,rp,gp,bp));
        }
    }

    printf("Running kmeans...\n");
    std::vector<Point>* meansPointer = kmeans(points, 100, 5.0);
    std::vector<Point>& means = *meansPointer;
    //printf("Means:\n");
    /*for(int i = 0; i < means.size(); i++) {
        means[i].print(); printf("\n");
        }*/

    Point* meansArray = &means[0];
    int numMeans = means.size();
    for(int i = 0; i < points.size(); i++) {
        Point* point = points[i];
        Point mean = means[getClosestMean(meansArray, numMeans, point)];
        int x = (int)rint(point->x/xweight);
        int y = (int)rint(point->y/yweight);
        int pixelLoc = 3*(y*image->w + x);
        image->data[pixelLoc+0] = (int)rint(mean.r);
        image->data[pixelLoc+1] = (int)rint(mean.g);
        image->data[pixelLoc+2] = (int)rint(mean.b);
        delete point;
    }
    delete meansPointer;

    printf("Displaying...\n");
    SDL_Window* window = SDL_CreateWindow("Test",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          image->w, image->h,
                                          SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf("Could not create window\n");
        return 1;
    }

    
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    if(screen == NULL) {
        printf("Could not load screen\n");
        return 1;
    }
    SDL_Surface* surface = convertToSurface(image);
    free(image->data);
    free(image);
    
    SDL_BlitSurface(surface, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);

    if(argc > 2) {
        printf("Saving image as %s\n", argv[2]);
        SDL_SaveBMP(surface,argv[2]);
    }
    
    SDL_Delay(5000);

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Success.\n");
    return 0;
}

