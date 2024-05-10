// IDENTIFIER  = 1761414855B69983BD8035097EFBD312EB0527F0

// Programmer: Xinyu Mei
// Date: Apr 4, 2024

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
#include "FASTTSP.h"
#include "OPTTSP.h"

class Drones {
    public:
        std::string mode;
        bool modeIsSpecified = false;
        bool mstMode = false;
        bool fasttspMode = false;
        bool opttspMode = false;
        void getOptions(int argc, char **argv);
};

int main(int argc, char** argv) {
    std::cout << std::setprecision(2); //Always show 2 decimal places
    std::cout << std::fixed; //Disable scientific notation for large numbers
    Drones drones;
    drones.getOptions(argc, argv);
    if (drones.mstMode) {
        MST mst;
        mst.read();
        mst.run();
        mst.print();
    }
    else if (drones.fasttspMode) {
        FASTTSP fst;
        fst.read();
        fst.run();
        fst.print();
    }
    else if (drones.opttspMode) {
        OPTTSP opt;
        opt.read();
        opt.run();
        opt.print();
    }
}

void Drones::getOptions(int argc, char **argv) {
    int option_index = 0, option = 0;
    opterr = false;

    struct option longOpts[] = {{ "mode", required_argument, nullptr, 'm'},
                                { "help", no_argument, nullptr, 'h' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "m:h", 
                                 longOpts, &option_index)) != -1) {
        switch (option) {
            case 'm':
                mode = optarg;
                modeIsSpecified = true;
                break;
            
            case 'h':
                std::cout << "This program reads a txt file that contains \n"
                          << "several commands, it then process the database \n"
                          << "and tables following the rules.\n"
                          << "Usage: \'./silly\n\t[--quiet | -q] <N>\n"
                          <<                      "\t[--help | -h]\n"
                          <<                      "\t< <TXT input map file>\n" 
                          <<                      "\t< <TXT output file>\n";
                exit(0);
            
            default:
                std::cerr << "Error: Invalid command line option\n";
                exit(1);
        }
    }
    if (!modeIsSpecified || mode.empty()) {
        std::cerr << "Error: No mode specified\n";
        exit(1);
    }
    if (mode == "MST") {
        mstMode = true;
    }
    else if (mode == "FASTTSP") {
        fasttspMode = true;
    }
    else if (mode == "OPTTSP") {
        opttspMode = true;
    }
    else {
        std::cerr << "Error: Invalid mode\n";
        exit(1);
    }
}
