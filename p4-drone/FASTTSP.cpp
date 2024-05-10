// IDENTIFIER  = 1761414855B69983BD8035097EFBD312EB0527F0
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

void FASTTSP::read() {
    int numPoints;
    std::cin >> numPoints;
    vertices.resize(numPoints);
    path.resize(numPoints);
    distances.resize(numPoints);
    int x;
    int y;
    for (int i = 0; i < numPoints; i++) {
        std::cin >> x >> y;
        vertices[i].x = x;
        vertices[i].y = y;
    }
}

double FASTTSP::distance(Point &pt1, Point &pt2) {
    return (static_cast<double>(pt1.x) - static_cast<double>(pt2.x)) * 
                     (static_cast<double>(pt1.x) - static_cast<double>(pt2.x)) +
                     (static_cast<double>(pt1.y) - static_cast<double>(pt2.y)) * 
                     (static_cast<double>(pt1.y) - static_cast<double>(pt2.y));
}

void FASTTSP::run() {
    double dist = std::sqrt(distance(vertices[0], vertices[1]));
    path[0] = 0;
    distances[0] = dist;
    path[1] = 1;
    distances[1] = dist;

    for (size_t i = 2; i < vertices.size(); i++) {
        double minDistance = std::numeric_limits<double>::infinity();
        int minIdx = -1;

        for (size_t j = 0; j < i; j++) {
            double currDist = std::sqrt(distance(vertices[i], vertices[path[j]])) + 
                              std::sqrt(distance(vertices[i], vertices[path[j+1]])) - 
                              distances[j];
            if (currDist < minDistance) {
                minDistance = currDist;
                minIdx = static_cast<int>(j);
            }
        }

        // Update the path and total distance
        double newDistL = std::sqrt(distance(vertices[i], vertices[path[minIdx]]));
        double newDistR = std::sqrt(distance(vertices[i], vertices[path[minIdx+1]]));

        distances[minIdx] = newDistL;
        path.insert(path.begin() + minIdx + 1, static_cast<int>(i));
        distances.insert(distances.begin() + minIdx + 1, newDistR);
    }
    path.resize(vertices.size());

    
    for (size_t i = 0; i < path.size(); i++) {
        fastTotal += distances[i];
    }
}

void FASTTSP::print() {
    std::cout << fastTotal << "\n";
    for (size_t i = 0; i < path.size(); i++) {
        std::cout << path[i] << " ";
    }
}