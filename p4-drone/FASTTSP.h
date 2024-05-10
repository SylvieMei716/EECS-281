// IDENTIFIER  = 1761414855B69983BD8035097EFBD312EB0527F0
#ifndef FASTTSP_H
#define FASTTSP_H
#include <cstddef>
#include <cstdio>
#include <limits>
#include <math.h>
#include <vector>
#include <deque>
#include <queue>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <map>

struct Point {
    int x = 0;
    int y = 0;
};

class FASTTSP {
    private:
        
    public:
        std::vector<Point> vertices;
        std::vector<int> path;
        std::vector<double> distances;
        double fastTotal = 0;
        void read();
        double distance(Point &pt1, Point &pt2);
        void run();
        void print();
};
#endif