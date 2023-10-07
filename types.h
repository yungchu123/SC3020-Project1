#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#ifndef TYPES_H
#define TYPES_H

// Define an address of a record stored as a block address with an offset
struct Address
{
    void *blockAddress;        // 8B. A pointer that points to block address
    unsigned short int offset; // 4B. An offset used to calculate the position of address within the block
};

// Define a single game record (read from data file)
struct GameRecord
{
    char GAME_DATE_EST[10];
    int TEAM_ID_home;
    int PTS_home;
    double FG_PCT_home;
    double FT_PCT_home;
    double FG3_PCT_home;
    int AST_home;
    int REB_home;
    int HOME_TEAM_WINS;
}; // Total 72B

#endif