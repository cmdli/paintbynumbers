
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "Point.h"

double getDistance(Point* p1, Point* p2) {
    double xd = p1->x-p2->x;
    double yd = p1->y-p2->y;
    double rd = p1->r-p2->r;
    double gd = p1->g-p2->g;
    double bd = p1->b-p2->b;
    return xd*xd + yd*yd + rd*rd + gd*gd + bd*bd;
}

int getClosestMean(Point* means, int numMeans, Point* point, int debug) {
    if(numMeans < 1)
        return -1;
    int closestMean = 0;
    double dist = getDistance(&means[0],point);//(means[0]-point).size();
    for(int meanI = 1; meanI < numMeans; meanI++) {
        double newDist = getDistance(&means[meanI],point);//(means[meanI]-point).size();
        if(newDist < dist) {
            closestMean = meanI;
            dist = newDist; 
        }
    }
    return closestMean;
}

int getClosestMean(Point* means, int numMeans, Point* point) {
    return getClosestMean(means, numMeans, point, 0);
}

std::vector<Point>* kmeans(std::vector<Point*>& data_points, 
                        int numMeans, 
                        double threshold) {
    printf("Num means: %d - Threshold: %f\n", numMeans, threshold);
    Point* means = new Point[numMeans];
    for(int i = 0; i < numMeans; i++) {
        means[i] = *data_points[(rand()%data_points.size())];
    }

    Point* newMeans = new Point[numMeans];
    double* counts = new double[numMeans];
    double dist = threshold*2.0;
    int iteration = 0;
    while(dist > threshold && iteration < 1000) {
        printf("Iteration: %d - Average distance: %f\n",iteration,dist);
        // Set new means to 0
        for(int i = 0; i < numMeans; i++) {
            newMeans[i] = Point();
            counts[i] = 0.0;
        }
        // Sum each point according to its cluster/mean
        int trigger = data_points.size()/10;
        for(int i = 0; i < data_points.size(); i++) {

            Point* point = data_points[i];
            int closestMean = getClosestMean(means, numMeans, point, i%trigger == 0);
            newMeans[closestMean] += point;
            counts[closestMean] += 1.0;
            /*if(i%trigger == 0) {
                printf("Point %d\n",i);
                printf("Closest mean: %d\n",closestMean);
                }*/
        }
        //printf("Calculating means\n");
        // Normalize means and calculate change from previous mean
        dist = 0.0;
        for(int i = 0; i < numMeans; i++) {
            if(counts[i] >= 0.5) {
                newMeans[i] /= counts[i];
                Point diff = (means[i]-newMeans[i]);
                dist += diff.size();
            } /*else {
                printf("Empty mean: %d - ",i); means[i].print(); printf("\n");
                }*/
        }
        Point* tmp = means;
        means = newMeans;
        newMeans = tmp;
        iteration += 1;
    }
    delete[] newMeans;
    delete[] counts;

    std::vector<Point>* returnMeans = new std::vector<Point>();
    for(int i = 0; i < numMeans; i++) {
        returnMeans->push_back(means[i]);
    }
    delete[] means;

    return returnMeans;
}

std::vector<Point>* kmeans(std::vector<Point*>& data_points, 
                        int numMeans) {
    return kmeans(data_points, numMeans, 0.00001);
}
