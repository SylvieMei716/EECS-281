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
#include "MST.h"

void MST::read() {
    int numPoints;
    bool medicalEmpty = true;
    bool mainEmpty = true;
    bool borderEmpty = true;
    std::cin >> numPoints;
    vertices.resize(numPoints);
    minDistance.resize(numPoints, std::numeric_limits<double>::infinity());
    preVertex.resize(numPoints);
    visited.resize(numPoints);

    for (int i = 0; i < numPoints; i++) {
        int x, y;
        std::cin >> x >> y;
        vertices[i].x = x;
        vertices[i].y = y;
        if (x < 0 && y < 0) {
            vertices[i].cate = PointLoc::medical;
            if (medicalEmpty) {
                medicalEmpty = false;
            }
        }
        else if ((x == 0 && y <= 0) || (y == 0 && x <= 0)) {
            vertices[i].cate = PointLoc::border;
            if (borderEmpty) {
                borderEmpty = false;
            }
        }
        else {
            vertices[i].cate = PointLoc::main;
            if (mainEmpty) {
                mainEmpty = false;
            }
        }
    }
    if (!medicalEmpty && !mainEmpty && borderEmpty) {
        std::cerr << "Cannot construct MST\n";
        exit(1);
    }
}

double MST::distance(PointLoc &loc1, PointLoc &loc2) {
    if ((loc1.cate == PointLoc::medical && loc2.cate == PointLoc::main) || 
        (loc1.cate == PointLoc::main && loc2.cate == PointLoc::medical)) {
            return std::numeric_limits<double>::infinity();
    }
    else {
        return (static_cast<double>(loc1.x) - static_cast<double>(loc2.x)) * 
                (static_cast<double>(loc1.x) - static_cast<double>(loc2.x)) + 
                         (static_cast<double>(loc1.y) - static_cast<double>(loc2.y)) * 
                         (static_cast<double>(loc1.y) - static_cast<double>(loc2.y));
    }
}

void MST::run() {
    
    int currVertexIdx = 0;
    minDistance[currVertexIdx] = 0;
    
    for (size_t i = 0; i < vertices.size(); i++) {
        double newEdge = std::numeric_limits<double>::infinity();
        for (size_t j = 0; j < vertices.size(); j++) {
            if (!visited[j]) {
                if (minDistance[j] < newEdge) {
                    newEdge = minDistance[j];
                    currVertexIdx = static_cast<int>(j);
                }
            }
        }
        visited[currVertexIdx] = true;
        totalEdge += std::sqrt(newEdge);
        for (size_t j = 0; j < vertices.size(); j++) {
            if (!visited[j]) {
                double dist = distance(vertices[currVertexIdx], vertices[j]);
                if (dist < minDistance[j]) {
                    minDistance[j] = dist;
                    preVertex[j] = currVertexIdx;
				}
            }
        }
    }
    
}

void MST::print() {
    std::cout << totalEdge << "\n";
    for (size_t i = 1; i < preVertex.size(); i++) {
        // int minNode = std::min(static_cast<int>(i), preVertex[i]);
        // int maxNode = std::max(static_cast<int>(i), preVertex[i]);
        std::cout << std::min(static_cast<int>(i), preVertex[i]) 
                  << " " << std::max(static_cast<int>(i), preVertex[i]) << "\n";
	}
}