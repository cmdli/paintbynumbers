
#ifndef POINT_H
#define POINT_H

class Point {
public:
    double x;
    double y;
    double r;
    double g;
    double b;

    Point() : x(0.0), y(0.0), r(0.0), g(0.0), b(0.0) {}
    Point(double _x, double _y, double _r, double _g, double _b) :
        x(_x), 
        y(_y), 
        r(_r), 
        g(_g), 
        b(_b) {}

    double dot(Point* p2) {
        return sqrt(x*p2->x + y*p2->y + r*p2->r + g*p2->g + b*p2->b);
    }

    double size() {
        return sqrt(x*x + y*y + r*r + g*g + b*b);
    }

    double similarity(Point* p2) {
        return (this->dot(this))/(this->size()*p2->size());
    }

    void print() {
        printf("(%f,%f,%f,%f,%f)",x,y,r,g,b);
    }

    void operator+=(Point p2) {
        x += p2.x;
        y += p2.y;
        r += p2.r;
        g += p2.g;
        b += p2.b;
    }
    void operator+=(Point* p2) {
        *this += (*p2);
    }
    void operator+=(double val) {
        x += val;
        y += val;
        r += val;
        g += val;
        b += val;
    }

    void operator*=(double scale) {
        x *= scale;
        y *= scale;
        r *= scale;
        g *= scale;
        b *= scale;
    }
    void operator/=(double scale) {
        *this *= (1.0/scale);
    }
    
    Point operator+(Point p2) {
        p2 += this;
        return p2;
    }

    Point operator-(Point p2) {
        Point copy(p2);
        copy.x = x-p2.x;
        copy.y = y-p2.y;
        copy.r = r-p2.r;
        copy.g = g-p2.g;
        copy.b = b-p2.b;
        return copy;
    }
};

int getClosestMean(Point* means, int numMeans, Point* _point);
std::vector<Point>* kmeans(std::vector<Point*>& data_points, 
                        int numMeans, 
                        double threshold);
std::vector<Point>* kmeans(std::vector<Point*>& data_points, 
                        int numMeans);

#endif
