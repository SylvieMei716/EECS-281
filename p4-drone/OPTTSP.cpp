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
#include "OPTTSP.h"

void OPTTSP::read() {
    fasttspHelper.read();
}

bool OPTTSP::promising(std::vector<int> &path, size_t &permLength) {
    if (path.size() - permLength < 5) {
        return true;
    }
    
    double distance = 0;
    for (size_t i = 0; i < permLength - 1; i++) {
        distance += std::sqrt(distanceMat[path[i]][path[i + 1]]);
    }
    if(distance > bestTotal)  {
	    return false;
    }

    // Initialize the prim vector and set the visited flag for the first node to true
    std::vector<bool> visited(vertices.size(), false);
    for (size_t i = 0; i < permLength; i++){
        visited[path[i]] = true;
    }
    double startDistance = std::numeric_limits<double>::infinity(); 
    double endDistance = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < vertices.size(); i++) {
        if (!visited[path[i]]) {
            startDistance = std::min(startDistance, distanceMat[path[0]][path[i]]);
            endDistance = std::min(endDistance, distanceMat[path[permLength - 1]][path[i]]);
        }
    }
    distance += std::sqrt(startDistance);
    distance += std::sqrt(endDistance);
    if(distance > bestTotal)  {
	    return false;
    }

    // Compute the minimum spanning tree for the path using Prim's algorithm
    double mstDistance = 0;
    std::vector<int> tempVertice;
    std::vector<bool> tempVisited;
    std::vector<double> minDistance;
    for (size_t i = 0; i < vertices.size(); i++) {
        if (!visited[path[i]]) {
            tempVertice.emplace_back(path[i]);
            tempVisited.emplace_back(false);
        }
    }
    minDistance.resize(tempVertice.size(), std::numeric_limits<double>::infinity());
    int currVertexIdx = 0;
    minDistance[currVertexIdx] = 0;
    
    for (size_t i = 0; i < tempVertice.size(); i++) {
        double newEdge = std::numeric_limits<double>::infinity();
        for (size_t j = 0; j < tempVertice.size(); j++) {
            if (!tempVisited[j]) {
                if (minDistance[j] < newEdge) {
                    newEdge = minDistance[j];
                    currVertexIdx = static_cast<int>(j);
                }
            }
        }
        tempVisited[currVertexIdx] = true;
        mstDistance += std::sqrt(newEdge);
        for (size_t j = 0; j < tempVertice.size(); j++) {
            if (!tempVisited[j]) {
                double dist = distanceMat[tempVertice[currVertexIdx]][tempVertice[j]];
                if (dist < minDistance[j]) {
                    minDistance[j] = dist;
				}
            }
        }
    }

    distance += mstDistance;

    if(distance > bestTotal)  {
	    return false;
    }
    return true;
}

void OPTTSP::genPerms(std::vector<int> &path, size_t &permLength) {
    //std::cout << permLength << std::endl;
    if (permLength == path.size()) {
        // Do something with the path
        // Calculate the distance of the current permutation
        double currTotal = 0;
        for (size_t i = 0; i < path.size() - 1; i++) {
            currTotal += std::sqrt(fasttspHelper.distance(vertices[path[i]], vertices[path[i + 1]]));
        }
        currTotal += std::sqrt(fasttspHelper.distance(vertices[path.back()], vertices[path.front()]));

        // Update the best path and distance if the current permutation is better
        if (currTotal < bestTotal) {
            bestPath = path;
            bestTotal = currTotal;
        }
        return;
    }  // if ..complete path

    if (!promising(path, permLength)) {
        //std::cout << permLength;
        return;
    }  // if ..not promising

    for (size_t i = permLength; i < path.size(); i++) {
        std::swap(path[permLength], path[i]);
        size_t permLength2 = permLength + 1;
        genPerms(path, permLength2);
        std::swap(path[permLength], path[i]);
    }  // for ..unpermuted elements
}  // genPerms()

void OPTTSP::run() {
    fasttspHelper.run();
    bestTotal = fasttspHelper.fastTotal;
    bestPath = fasttspHelper.path;
    vertices = fasttspHelper.vertices;
    std::vector<int> currPath = bestPath;
    distanceMat.resize(vertices.size());

    for(size_t i = 0; i < vertices.size(); i++) {
        std::vector<double> row;
        row.resize(vertices.size());
        for(size_t j = 0; j < vertices.size(); j++) {
            row[j] = fasttspHelper.distance(vertices[i], vertices[j]);
        }
        distanceMat[i] = row;
    }
    size_t permLength = 1;
    genPerms(currPath, permLength);
}

void OPTTSP::print() {
    std::cout << bestTotal << "\n";
    for (size_t i = 0; i < bestPath.size(); i++) {
        std::cout << bestPath[i] << " ";
    }
}