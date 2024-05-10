// Project Identifier: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F

// Date: Feb 7, 2024
// Programmer: Xinyu Mei
// This program will take in a map of the mine and the output mode.
// And its goal is to help the miner to escape from the mine and print the path. 

#include <cstddef>
#include <sstream>
#include <vector>
#include <deque>
#include <queue>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <string>
#include "P2random.h"
#include <iomanip>


struct tileMap {
    int rubble = 0;
    bool discovered = false;
    bool investigated = false;
};

struct tileInfo {
    int rubble = 0;
    size_t row = 0;
    size_t col = 0;
};

struct tileComp {
    bool operator()(const tileInfo &lhs, const tileInfo &rhs) {
        if ((lhs.rubble == rhs.rubble) && (lhs.col == rhs.col)) {
            return lhs.row > rhs.row;
        }
        else if (lhs.rubble == rhs.rubble) {
            return lhs.col > rhs.col;
        }
        else {
            return lhs.rubble > rhs.rubble;
        }
    }
};

struct tileCompOppo {
    bool operator()(const tileInfo &lhs, const tileInfo &rhs) {
        if ((lhs.rubble == rhs.rubble) && (lhs.col == rhs.col)) {
            return lhs.row < rhs.row;
        }
        else if (lhs.rubble == rhs.rubble) {
            return lhs.col < rhs.col;
        }
        else {
            return lhs.rubble < rhs.rubble;
        }
    }
};

class MineEscapeClass{
    private:
        bool statsIsSpecified = false;
        size_t tilesToPrint;
        bool medianIsSpecified = false;
        bool verboseIsSpecified = false;
        size_t currentRow;
        size_t currentCol;

    public:
        std::vector<std::vector<tileMap>> map2D;
        size_t startRow;
        size_t startCol;

        std::vector<tileInfo> statsPath;
        int numTileCleared = 0;
        int numRubbleCleared = 0;

        std::priority_queue<int, std::vector<int>> leftPQ;
        std::priority_queue<int, std::vector<int>, std::greater<int>> rightPQ;
        

        std::priority_queue<tileInfo, std::vector<tileInfo>, tileComp> primaryPQ;
        std::priority_queue<tileInfo, std::vector<tileInfo>, tileComp> TNTPQ;

        void getOptions(int argc, char** argv);
        void readMine();
        void updateMedians(int newVal);
        void printMedian();
        void clear();
        void updatePQ();
        void updateTNTPQ();
        void TNTExplode();
        bool isOnEdge(const struct tileMap &tile, size_t &row, size_t &col);
        void escape();
        void output();
};

int main(int argc, char** argv) {
    std::cout << std::fixed << std::setprecision(2);
    MineEscapeClass mine;
    mine.getOptions(argc, argv);

    mine.readMine();

    mine.escape();

    mine.output();

    return 0;
}

void MineEscapeClass::getOptions(int argc, char **argv) {
    int option_index = 0, option = 0;
    opterr = false;

    struct option longOpts[] = {{ "stats", required_argument, nullptr, 's'},
                                { "median", no_argument, nullptr, 'm'},
                                { "verbose", no_argument, nullptr, 'v'},
                                { "help", no_argument, nullptr, 'h' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "s:mvh", 
                                 longOpts, &option_index)) != -1) {
        switch (option) {
            case 's':
                statsIsSpecified = true;
                tilesToPrint = atoi(optarg);
                break;       

            case 'm':
                medianIsSpecified = true;
                break;
                
            case 'v':
                verboseIsSpecified = true;
                break;
            
            case 'h':
                std::cout << "This program reads a txt file that contains a \n"
                          << "map, it then outputs the path to excape from \n"
                          << "the mine following the rules specified in"
                          << " command line arguments.\n"
                          << "Usage: \'./project2\n\t[--stats | -s] <N>\n"
                          <<                      "\t[--median | -m]\n"
                          <<                      "\t[--verbose | -v]\n"
                          <<                      "\t[--help | -h]\n"
                          <<                      "\t< <TXT input map file>\n" 
                          <<                      "\t< <TXT output file>\n";    
                exit(0);
        }
    }
}

void MineEscapeClass::readMine() {
    std::string inputType;
    std::string sizeTag;
    std::string posTag;
    size_t sizeMine;

    std::cin >> inputType >> sizeTag >> sizeMine >> posTag 
             >> startRow >> startCol;

    if (startRow >= sizeMine) {
        std::cerr << "Invalid starting row\n";
        exit(1);
    }
    if (startCol >= sizeMine) {
        std::cerr << "Invalid starting column\n";
        exit(1);
    }

    map2D.resize(sizeMine, std::vector<tileMap>(sizeMine));
    if (inputType != "M" && inputType != "R") {
        std::cerr << "Invalid input mode\n";
        exit(1); 
    }

    std::stringstream ss;

    if (inputType == "R") {
        //readrandom
        std::string seedTag;
        std::string rubbleTag;
        std::string tntTag;
        uint32_t seed;
        uint32_t max_rubble;
        uint32_t tnt_chance;

        std::cin >> seedTag >> seed >> rubbleTag >> max_rubble 
                 >> tntTag >> tnt_chance;

        P2random::PR_init(ss, static_cast<uint32_t>(sizeMine), 
                          seed, max_rubble, tnt_chance);

    }

    std::istream& inputStream = (inputType == "M") ? std::cin : ss;

    std::string intermediate;

    for (size_t i = 0; i < map2D.size(); ++i) {
        for (size_t j = 0; j < map2D.size(); ++j) {        
            inputStream >> intermediate;
            map2D[i][j].rubble = stoi(intermediate);
        }
    }
}

void MineEscapeClass::updateMedians(int newVal) {
    // update median heaps
    if (leftPQ.size() == rightPQ.size()) {
        if (leftPQ.empty() || newVal <= rightPQ.top()) {
            leftPQ.push(newVal);
        }
        else {
            int temp = rightPQ.top();
            rightPQ.pop();
            rightPQ.push(newVal);
            leftPQ.push(temp);
        }
    }
    else {
        if (rightPQ.empty()) {
            if (leftPQ.top() <= newVal) {
                rightPQ.push(newVal);
            } 
            else {
                int temp = leftPQ.top();
                leftPQ.pop();
                leftPQ.push(newVal);
                rightPQ.push(temp);
            }
        }
        else if (newVal >= leftPQ.top()) {
            rightPQ.push(newVal);
        }
        else {
            int temp = leftPQ.top();
            leftPQ.pop();
            leftPQ.push(newVal);
            rightPQ.push(temp);
        }
    }
}

void MineEscapeClass::printMedian() {
    //print median
    std::cout << "Median difficulty of clearing rubble is: ";
    int size = static_cast<int>(leftPQ.size() + rightPQ.size());

    if (size % 2 == 0) {
        std::cout << static_cast<double>(leftPQ.top() + rightPQ.top()) / 2 << "\n";
    }
    else {
        std::cout << static_cast<double>(leftPQ.top()) << "\n";
    }
}

void MineEscapeClass::updatePQ() {
    // update primary pq
    if((currentRow + 1 != map2D.size()) && 
       (!map2D[currentRow + 1][currentCol].discovered)) {
        if(!((map2D[currentRow + 1][currentCol].rubble == -1) && 
             (map2D[currentRow][currentCol].rubble == -1))) {
            map2D[currentRow + 1][currentCol].discovered = true;

            tileInfo newTile;
            newTile.rubble = map2D[currentRow + 1][currentCol].rubble;
            newTile.row = currentRow + 1;
            newTile.col = currentCol;
            primaryPQ.push(newTile);
        }
    }

    if((currentRow != 0) && 
       (!map2D[currentRow - 1][currentCol].discovered)) {
        if(!((map2D[currentRow - 1][currentCol].rubble == -1) && 
             (map2D[currentRow][currentCol].rubble == -1))) {
            map2D[currentRow - 1][currentCol].discovered = true;

            tileInfo newTile;
            newTile.rubble = map2D[currentRow - 1][currentCol].rubble;
            newTile.row = currentRow - 1;
            newTile.col = currentCol;
            primaryPQ.push(newTile);
        }
    }

    if((currentCol + 1 != map2D.size()) && 
       (!map2D[currentRow][currentCol + 1].discovered)) {
        if(!((map2D[currentRow][currentCol + 1].rubble == -1) && 
             (map2D[currentRow][currentCol].rubble == -1))){
            map2D[currentRow][currentCol + 1].discovered = true;

            tileInfo newTile;
            newTile.rubble = map2D[currentRow][currentCol + 1].rubble;
            newTile.row = currentRow;
            newTile.col = currentCol + 1;
            primaryPQ.push(newTile);
        }
    }

    if((currentCol != 0) && 
       (!map2D[currentRow][currentCol - 1].discovered)) {
        if(!((map2D[currentRow][currentCol - 1].rubble == -1) && 
        (map2D[currentRow][currentCol].rubble == -1))){
            map2D[currentRow][currentCol - 1].discovered = true;

            tileInfo newTile;
            newTile.rubble = map2D[currentRow][currentCol - 1].rubble;
            newTile.row = currentRow;
            newTile.col = currentCol - 1;
            primaryPQ.push(newTile);
        }
    }
}

void MineEscapeClass::updateTNTPQ() {
    // update tnt pq
    if((currentRow + 1 != map2D.size()) && 
       (!map2D[currentRow + 1][currentCol].investigated)) {
        map2D[currentRow + 1][currentCol].investigated = true;

        tileInfo newTile;
        newTile.rubble = map2D[currentRow + 1][currentCol].rubble;
        newTile.row = currentRow + 1;
        newTile.col = currentCol;

        TNTPQ.push(newTile);
        if (map2D[currentRow + 1][currentCol].rubble >= 0) {
            newTile.rubble = 0;
            primaryPQ.push(newTile);
        }
    }

    if((currentRow != 0) && 
       (!map2D[currentRow - 1][currentCol].investigated)) {
        map2D[currentRow - 1][currentCol].investigated = true;

        tileInfo newTile;
        newTile.rubble = map2D[currentRow - 1][currentCol].rubble;
        newTile.row = currentRow - 1;
        newTile.col = currentCol;

        TNTPQ.push(newTile);
        if (map2D[currentRow - 1][currentCol].rubble >= 0) {
            newTile.rubble = 0;
            primaryPQ.push(newTile);
        }
    }

    if((currentCol + 1 != map2D.size()) && 
       (!map2D[currentRow][currentCol + 1].investigated)) {
        map2D[currentRow][currentCol + 1].investigated = true;
        
        tileInfo newTile;
        newTile.rubble = map2D[currentRow][currentCol + 1].rubble;
        newTile.row = currentRow;
        newTile.col = currentCol + 1;

        TNTPQ.push(newTile);
        if (map2D[currentRow][currentCol + 1].rubble >= 0) {
            newTile.rubble = 0;
            primaryPQ.push(newTile);
        }
    }

    if((currentCol != 0) && 
       (!map2D[currentRow][currentCol - 1].investigated)) {
        map2D[currentRow][currentCol - 1].investigated = true;
        
        tileInfo newTile;
        newTile.rubble = map2D[currentRow][currentCol - 1].rubble;
        newTile.row = currentRow;
        newTile.col = currentCol - 1;

        TNTPQ.push(newTile);
        if (map2D[currentRow][currentCol - 1].rubble >= 0) {
            newTile.rubble = 0;
            primaryPQ.push(newTile);
        }
    }
}

void MineEscapeClass::TNTExplode() {
    // tnt explosion
    if (!TNTPQ.empty()) {
        while (TNTPQ.top().rubble == -1 && !TNTPQ.empty()) {
            if (verboseIsSpecified) {
                std::cout << "TNT explosion at " << "[" << TNTPQ.top().row 
                          << "," << TNTPQ.top().col << "]!\n";
            }
            if (statsIsSpecified) {
                statsPath.emplace_back(TNTPQ.top());
            }
            currentRow = TNTPQ.top().row;
            currentCol = TNTPQ.top().col;
            map2D[currentRow][currentCol].rubble = 0;
            map2D[currentRow][currentCol].discovered = true;
            TNTPQ.pop();
            updateTNTPQ();
        }
    }
    while (!TNTPQ.empty()) {
        if (verboseIsSpecified && TNTPQ.top().rubble != 0) {
            std::cout << "Cleared by TNT: " << TNTPQ.top().rubble << " at "
                      << "[" << TNTPQ.top().row << "," 
                      << TNTPQ.top().col << "]\n";
        }
        if ((medianIsSpecified) && (TNTPQ.top().rubble > 0)) {
            updateMedians(map2D[TNTPQ.top().row][TNTPQ.top().col].rubble);
            printMedian();
        }
        if (statsIsSpecified && 
            map2D[TNTPQ.top().row][TNTPQ.top().col].rubble > 0) {
            statsPath.emplace_back(TNTPQ.top());
        }
        if (map2D[TNTPQ.top().row][TNTPQ.top().col].rubble != 0) {
            numTileCleared++;
        }
        currentRow = TNTPQ.top().row;
        currentCol = TNTPQ.top().col;
        numRubbleCleared += map2D[currentRow][currentCol].rubble;
        map2D[currentRow][currentCol].rubble = 0;
        tileInfo newTile;
        newTile.rubble = 0;
        newTile.row = currentRow;
        newTile.col = currentCol;
        primaryPQ.push(newTile);
        TNTPQ.pop();
    }
    if (isOnEdge(map2D[currentRow][currentCol], currentRow, currentCol) && primaryPQ.empty()) {
        return;
    }
    currentRow = primaryPQ.top().row;
    currentCol = primaryPQ.top().col;
}

void MineEscapeClass::clear() {
    // clear adjacent tiles
    if (map2D[currentRow][currentCol].investigated) {
        primaryPQ.pop();
        return;
    }
    if (map2D[currentRow][currentCol].rubble == -1) {
        if (verboseIsSpecified) {
            std::cout << "TNT explosion at [" << currentRow 
                      << "," << currentCol << "]!\n";
        }
        if (statsIsSpecified) {
            statsPath.emplace_back(primaryPQ.top());
            
        }
        map2D[currentRow][currentCol].investigated = true;
        map2D[currentRow][currentCol].rubble = 0;
        primaryPQ.pop();
        updateTNTPQ();
        TNTExplode();
    }
    else {
        if ((verboseIsSpecified) && (primaryPQ.top().rubble > 0)) {
            std::cout << "Cleared: " << primaryPQ.top().rubble << " at [" 
                      << primaryPQ.top().row << "," 
                      << primaryPQ.top().col << "]\n";
        }
        if ((medianIsSpecified) && (primaryPQ.top().rubble > 0)) {
            updateMedians(primaryPQ.top().rubble);
            printMedian();
        }
        if (statsIsSpecified && primaryPQ.top().rubble > 0) {
            statsPath.emplace_back(primaryPQ.top());
        }
        numRubbleCleared += map2D[currentRow][currentCol].rubble;
        if (map2D[currentRow][currentCol].rubble != 0) {
            numTileCleared++;
        }
        map2D[currentRow][currentCol].rubble = 0;
        map2D[currentRow][currentCol].investigated = true;
        currentRow = primaryPQ.top().row;
        currentCol = primaryPQ.top().col;
        primaryPQ.pop();
    }
}

bool MineEscapeClass::isOnEdge(const struct tileMap &tile, size_t &row, size_t &col) {
    // if tile is on edge
    if (((col == map2D.size() - 1) || (col == 0) || 
        (row == map2D.size() - 1) || (row == 0)) && tile.rubble == 0) {
        return true;
    }
    else {
        return false;
    }
}

void MineEscapeClass::escape() {

    tileInfo newTile;
    newTile.rubble = map2D[startRow][startCol].rubble;
    newTile.row = startRow;
    newTile.col = startCol;
    primaryPQ.push(newTile);
    currentRow = startRow;
    currentCol = startCol;
    map2D[startRow][startCol].discovered = true;


    while (!isOnEdge(map2D[currentRow][currentCol], currentRow, currentCol)) {
        clear();
        if ((currentCol == map2D.size() - 1) || (currentCol == 0) || 
        (currentRow == map2D.size() - 1) || (currentRow == 0)) {
            break;
        }
        updatePQ();
        currentRow = primaryPQ.top().row;
        currentCol = primaryPQ.top().col;
    }
}

void MineEscapeClass::output() {

    std::cout << "Cleared " << numTileCleared << " tiles containing " 
              << numRubbleCleared << " rubble and escaped.\n";
    if (!statsIsSpecified) {
        return;
    }
    else {
        std::cout << "First tiles cleared:\n";
        if (!statsPath.empty()) {
            for (size_t i = 0; i < tilesToPrint; i++) {
                if (i == statsPath.size()) {
                    break;
                }
                if (statsPath[i].rubble > 0) {
                    std::cout << statsPath[i].rubble << " at [" 
                              << statsPath[i].row << ","
                              << statsPath[i].col << "]\n";
                }
                else {
                    std::cout << "TNT at [" << statsPath[i].row << "," 
                              << statsPath[i].col << "]\n";
                }
            }
        }
        std::cout << "Last tiles cleared:\n";
        if (!statsPath.empty()) {
            for (int i = static_cast<int>(statsPath.size() - 1); 
                 (static_cast<int>(statsPath.size())) - i - 1 < 
                  (static_cast<int>(tilesToPrint)); i--) {
            // for (size_t i = statsPath.size() - 1; 
                // i > statsPath.size() - tilesToPrint - 1; i--) {
                if (i == -1) {
                    break;
                }
                if (statsPath[i].rubble > 0) {
                    std::cout << statsPath[i].rubble << " at [" 
                              << statsPath[i].row << ","
                              << statsPath[i].col << "]\n";
                }
                else {
                    std::cout << "TNT at [" << statsPath[i].row << "," 
                              << statsPath[i].col << "]\n";
                }
            }
        }

        std::priority_queue<tileInfo, std::vector<tileInfo>, tileComp> easyPQ;
        std::priority_queue<tileInfo, std::vector<tileInfo>, tileCompOppo> hardPQ;
        for (size_t i = 0; i < statsPath.size(); i++) {
            easyPQ.push(statsPath[i]);
            hardPQ.push(statsPath[i]);
        }

        std::cout << "Easiest tiles cleared:\n";
        if (!statsPath.empty()) {
            for (size_t i = 0; i < tilesToPrint; i++) {
                if (i == statsPath.size()) {
                    break;
                }
                if (easyPQ.top().rubble > 0) {
                    std::cout << easyPQ.top().rubble << " at [" 
                              << easyPQ.top().row << ","
                              << easyPQ.top().col << "]\n";
                }
                else {
                    std::cout << "TNT at [" << easyPQ.top().row << "," 
                              << easyPQ.top().col << "]\n";
                }
                easyPQ.pop();
            }
        }
        std::cout << "Hardest tiles cleared:\n";
        if (!statsPath.empty()) {
            for (size_t i = 0; i < tilesToPrint; i++) {
                if (i == statsPath.size()) {
                    break;
                }
                if (hardPQ.top().rubble > 0) {
                    std::cout << hardPQ.top().rubble << " at [" 
                              << hardPQ.top().row << ","
                              << hardPQ.top().col << "]\n";
                }
                else {
                    std::cout << "TNT at [" << hardPQ.top().row << "," 
                              << hardPQ.top().col << "]\n";
                }
                hardPQ.pop();
            }
        }
    }
}