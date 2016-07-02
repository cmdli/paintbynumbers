
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "Point.h"

static inline double getDistance(Point* p1, Point* p2) {
    double xd = p1->x-p2->x;
    double yd = p1->y-p2->y;
    double rd = p1->r-p2->r;
    double gd = p1->g-p2->g;
    double bd = p1->b-p2->b;
    return xd*xd + yd*yd + rd*rd + gd*gd + bd*bd;
}

int getClosestMean(Point* means, int numMeans, Point* point, long& timeUsed) {
    long start = clock();
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
    timeUsed += clock() - start;
    return closestMean;
}

int getClosestMean(Point* means, int numMeans, Point* point) {
    long timeUsed = 0;
    return getClosestMean(means, numMeans, point, timeUsed);
}

inline void addToPoint(Point& p1, Point& p2) {
    p1.x += p2.x;
    p1.y += p2.y;
    p1.r += p2.r;
    p1.g += p2.g;
    p1.b += p2.b;
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
    long time1,time2,time3;
    long timeDiff;
    while(dist > threshold && iteration < 1000) {
        time1 = time2 = time3 = 0;
        printf("Iteration: %d - Average distance: %f\n",iteration,dist);
        long iterationStart = clock();

        // Set new means to 0
        timeDiff = clock();
        for(int i = 0; i < numMeans; i++) {
            newMeans[i] = Point();
            counts[i] = 0.0;
        }
        time3 += clock() - timeDiff;

        // Sum each point according to its cluster/mean
        int trigger = data_points.size()/10;
        long timeUsed = 0;
        for(int i = 0; i < data_points.size(); i++) {

            Point* point = data_points[i];

            timeDiff = clock();
            int closestMean = getClosestMean(means, numMeans, point, timeUsed);
            time1 += clock() - timeDiff;

            timeDiff = clock();
            addToPoint(newMeans[closestMean],*point);
            time2 += clock() - timeDiff;

            //newMeans[closestMean] += point;
            counts[closestMean] += 1.0;
        }

        // Normalize means and calculate change from previous mean
        dist = 0.0;
        for(int i = 0; i < numMeans; i++) {
            if(counts[i] >= 0.5) {
                newMeans[i] /= counts[i];
                Point diff = (means[i]-newMeans[i]);
                dist += diff.size();
            }
        }

        Point* tmp = means;
        means = newMeans;
        newMeans = tmp;

        printf("time1: %lums\ntime2: %lums\ntime3: %lums\n",
            time1/1000,
            time2/1000,
            time3/1000);
        printf("Total time: %lums\n",(clock()-iterationStart)/1000);
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
