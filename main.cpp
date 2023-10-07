#include "memory_pool.h"
#include "types.h"
#include "b_plus_tree.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


int main()
{
    std::cout << "Hello world\n";

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

    // Create the tree 
    std::cout << "Creating the tree" << std::endl;
    BPlusTree tree = BPlusTree(BLOCKSIZE, &disk);

    // Open test data
    std::cout <<"Reading in data ... "<< std::endl;
    std::ifstream datafile("games.txt");

    std::vector<GameRecord> gameRecordList;
    std::vector<Address> recordAddressList;
    int recordNum = 0;

    if (datafile.is_open())
    {
        std::string line;

        // Skip the header line
        getline(datafile, line);

        while (getline(datafile, line))
        {
            GameRecord gameRecord;
            std::stringstream ss(line);

            // Assigning value to record
            ss >> gameRecord.GAME_DATE_EST >> gameRecord.TEAM_ID_home >> gameRecord.PTS_home >> gameRecord.FG_PCT_home >> gameRecord.FT_PCT_home >> gameRecord.FG3_PCT_home >> gameRecord.AST_home >> gameRecord.REB_home >> gameRecord.HOME_TEAM_WINS;
            gameRecordList.push_back(gameRecord);

            // insert record to database
            Address tempAddress = disk.saveToDisk(&gameRecord, sizeof(GameRecord));
            recordAddressList.push_back(tempAddress);

            recordNum++;
        }
        datafile.close();
    }

    // for (const GameRecord &game : gameRecordList)
    // {
    //     std::cout << "ID: " << game.TEAM_ID_home << ", Points: " << game.PTS_home << ", Size: " << sizeof(game) << std::endl;
    // }
    std::cout << gameRecordList[1].FG_PCT_home << std::endl;
    std::cout << recordAddressList[1].blockAddress << std::endl;
    std::cout <<"=====================================Experiment 1=========================================="<< std::endl;
    std::cout << "Number of Records: " << recordNum << std::endl;
    std::cout << "Size of a Record: " << sizeof(GameRecord) << std::endl;
    std::cout << "Number of Records in a Block: " << BLOCKSIZE / sizeof(GameRecord) << std::endl;
    std::cout << "Number of Blocks for storing data: " << disk.getAllocated() << std::endl;
    std::cout << std::endl;
    
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

            /*
    =============================================================
    Experiment 2:
    Build a B+ tree on the attribute "FG_PCT_home" by inserting the records sequentially and report the following statistics:
    - the parameter n of the B+ tree
    - the number of nodes of the B+ tree
    - the number of levels of the B+ tree
    - the content of the root node (only the keys)
    =============================================================
    */

    // Build the B+ tree from the records
    for (int i = 0 ; i < 70 ; i++) {
        std::cout << i ;
        tree.insert(recordAddressList[i], gameRecordList[i].FG3_PCT_home); // FG3_PCT_home is used as our primary key
    }

    std::cout <<"=====================================Experiment 2=========================================="<< std::endl;
    std::cout << "Parameter n of the B+ tree     : "<<tree.getMaxKeys()<< std::endl;
    std::cout << "Number of nodes of the B+ tree : "<<tree.getNumNodes()<< std::endl;
    std::cout << "Height of the B+ tree          : "<<tree.getLevels()<< std::endl;
    std::cout << "Root node :"<< std::endl;
    tree.displayNode(tree.getRootOfTree());
    std::cout << std::endl;
 
                /*
    =============================================================
    Experiment 3:
    Retrieve those movies with the "FG_PCT_home" equal to 0.5 and report the following statistics:
    - the number of index nodes the process accesses
    - the number of data blocks the process accesses
    - the average of “FG3_PCT_home” of the records that are returned
    - the running time of the retrieval process (please specify the method you use for measuring the running time of a piece of code)
    - the number of data blocks that would be accessed by a brute-force linear scan method (i.e., it scans the data blocks one by one) and its running time (for comparison)
    =============================================================
    */

    std::cout <<"=====================================Experiment 3=========================================="<<endl;
    tree.search(0.5, 0.5);
    std::cout << std::endl;

               /*
    =============================================================
    Experiment 4:
    Retrieve those movies with the attribute "FG_PCT_home" from 0.6 to 1, both inclusively and report the following statistics:
    - the number of index nodes the process accesses
    - the number of data blocks the process accesses
    - the average of “FG3_PCT_home” of the records that are returned
    - the running time of the retrieval process
    - the number of data blocks that would be accessed by a brute-force linear scan method (i.e., it scans the data blocks one by one) and its running time (for comparison)
    =============================================================
    */

    std::cout <<"=====================================Experiment 4=========================================="<<endl;
    tree.search(0.6, 1);
    std::cout << std::endl;

               /*
    =============================================================
    Experiment 5:
    Delete those movies with the attribute “FG_PCT_home” below 0.35 inclusively, update the B+ tree accordingly, and report the following statistics:
    - the number nodes of the updated B+ tree
    - the number of levels of the updated B+ tree
    - the content of the root node of the updated B+ tree(only the keys)
    - the running time of the process;
    - the number of data blocks that would be accessed by a brute-force linear scan method (i.e., it scans the data blocks one by one) and its running time (for comparison)
    =============================================================
    */

    return 0;
}