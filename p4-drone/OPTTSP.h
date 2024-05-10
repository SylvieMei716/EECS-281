// IDENTIFIER  = 1761414855B69983BD8035097EFBD312EB0527F0
#ifndef OPTTSP_H
#define OPTTSP_H
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
#include "FASTTSP.h"

class OPTTSP {
    private:
        std::vector<std::vector<double>> distanceMat;
        std::vector<int> bestPath;
        std::vector<Point> vertices;
        double bestTotal = std::numeric_limits<double>::infinity();
        FASTTSP fasttspHelper;

    public:
        void read();
        void genPerms(std::vector<int> &path, size_t &permLength);
        bool promising(std::vector<int> &path, size_t &permLength);
        void run();
        void print();
};

#endif