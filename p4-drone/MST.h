// IDENTIFIER  = 1761414855B69983BD8035097EFBD312EB0527F0
#ifndef MST_H
#define MST_H

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

struct PointLoc {
    enum Category{medical, main, border};
    int x = 0;
    int y = 0;
    Category cate = main;
};

class MST {
    private:
        std::vector<PointLoc> vertices;
        std::vector<double> minDistance;
	    std::vector<int> preVertex;
	    std::vector<bool> visited;
        double totalEdge = 0;

    public:
        void read();
        double distance(PointLoc &loc1, PointLoc &loc2);
        void run();
        void print();
};

#endif