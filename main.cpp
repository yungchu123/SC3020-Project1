#include "memory_pool.h"
#include "types.h"
#include "b_plus_tree.h"
#include "b_plus_tree_node.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


int main()
{
    // Initialise
    int BLOCKSIZE = 400;             // 400B
    int MEMORYPOOLSIZE = 500000000;  // 500MB

        /*
    =============================================================
    Experiment 1:
    Store the data (which is about NBA games and described in Part 4) on the disk (as specified in Part 1) and report the following statistics
    - the number of records;
    - the size of a record;
    - the number of records stored in a block;
    - the number of blocks for storing the data;
    =============================================================
    */

    // Create memory pool for the disk
    std::cout << "Creating the disk on the stack for records, index" << std::endl;
    MemoryPool disk(MEMORYPOOLSIZE, BLOCKSIZE);

    // Open test data
    std::cout <<"Reading in data ... "<< std::endl;
    std::ifstream datafile("games.txt");

    std::vector<GameRecord> gameRecordList;

    // Initialising B+ Tree
    BPlusTree tree = BPlusTree(BLOCKSIZE, &disk);

    if (datafile.is_open())
    {
        std::string line;
        int recordNum = 0;

        // Skip the header line
        getline(datafile, line);

        while (getline(datafile, line) && recordNum < 50)
        {
            GameRecord gameRecord;
            std::stringstream ss(line);

            // Assigning value to record
            ss >> gameRecord.GAME_DATE_EST >> gameRecord.TEAM_ID_home >> gameRecord.PTS_home >> gameRecord.FG_PCT_home >> gameRecord.FT_PCT_home >> gameRecord.FG3_PCT_home >> gameRecord.AST_home >> gameRecord.REB_home >> gameRecord.HOME_TEAM_WINS;
            gameRecordList.push_back(gameRecord);

            // insert record to database
            Address* tempAddress = new Address;
            *tempAddress = disk.saveToDisk(&gameRecord, sizeof(GameRecord));
            
            // build b+ tree as we insert records
            std::cout << "Key number = " << recordNum+1 << ": ";
            tree.insert(tempAddress, gameRecord.PTS_home);

            recordNum++;
        }
        datafile.close();
    }

    // for (const GameRecord &game : gameRecordList)
    // {
    //     std::cout << "ID: " << game.TEAM_ID_home << ", Points: " << game.PTS_home << ", Size: " << sizeof(game) << std::endl;
    // }

    std::cout <<"=====================================Experiment 1=========================================="<< std::endl;
    std::cout << "Number of Records: " << disk.getActualSizeUsed() / sizeof(GameRecord) << std::endl;
    std::cout << "Size of a Record: " << sizeof(GameRecord) << std::endl;
    std::cout << "Number of Records in a Block: " << BLOCKSIZE / sizeof(GameRecord) << std::endl;
    std::cout << "Number of Blocks for storing data: " << disk.getAllocated() << std::endl;
    
    // OTHER LOGGING INFO --------------------------
    // std::cout << "Number of keys per index block --- " << tree.getMaxKeys() << endl;
    // std::cout << "Number of index blocks --- " << index.getAllocated() << endl;
    //std::cout << "Size of actual record data stored --- " << disk.getActualSizeUsed() << std::endl;
    // std::cout << "Size of actual index data stored --- " << index.getActualSizeUsed() << endl;
    //std::cout << "Size of record blocks --- " << disk.getSizeUsed() << std::endl;
    // std::cout << "Size of index blocks --- " << index.getSizeUsed() << endl;
    // std::cout <<"Total number of blocks   : "<< disk.getAllocated() + index.getAllocated()<<endl;
    // std::cout <<"Actual size of database : "<< disk.getActualSizeUsed() + index.getActualSizeUsed()<<endl;
    //std::cout <<"Size of database (size of all blocks): "<<disk.getSizeUsed()<< std::endl;


    return 0;
}