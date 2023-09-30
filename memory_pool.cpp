#include "memory_pool.h"
#include "types.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>

// We need to create a general memory pool that can be used for both the relational data and the index.
// This pool should be able to assign new blocks if necessary.

// Constructor

MemoryPool::MemoryPool(std::size_t maxPoolSize, std::size_t blockSize) {
    this->maxPoolSize = maxPoolSize;
    this->blockSize = blockSize;
    this->sizeUsed = 0;                 // Total size occupied by blocks
    this->actualSizeUsed = 0;           // Total size occupied by records
    this->allocated = 0;                // Number of blocks that have been initialised

    this->pool = operator new(maxPoolSize);     // Allocate memory to the MemoryPool object
    memset(pool, '\0', maxPoolSize);       // Initialise all pool to null.
    this->block = nullptr;
    this->blockSizeUsed = 0;        // A variable that keeps track of the current block size used so that we can allocate to the right address
    this->blocksAccessed = 0;       // A variable that keeps track of the total number of times the blocks have been accessed
}

bool MemoryPool::allocateBlock(){
    if(sizeUsed + blockSize <= maxPoolSize){
        block = (char *)pool + (allocated * blockSize);
        sizeUsed += blockSize;
        blockSizeUsed = 0;
        allocated++;
        return true;
    }
    else{
        std::cout << "Error: No memory left to allocate new block " << '\n';
        return false;
    }
}

Address MemoryPool::allocate(std::size_t sizeRequired){
    // If record size exceeds block size, throw an error
    if(sizeRequired > blockSize){
        std::cout << "Error: Requested Record size too large! (Requested: " << sizeRequired << " vs Block: " << blockSize << ")" << '\n';
        throw std::invalid_argument("Requested size too large");
    }

    // If no current block or record cant fit into current block, make a new block
    if (allocated == 0 || blockSizeUsed + sizeRequired > blockSize) {
        bool isSuccessful = allocateBlock();
        if (!isSuccessful) {
            throw std::logic_error("Failed to allocate new block");
        }
    }

    // Update variables
    unsigned short int offset = blockSizeUsed;
    blockSizeUsed += sizeRequired;
    actualSizeUsed += sizeRequired;

    Address recordAddress = {block, offset};
    return recordAddress;
}

bool MemoryPool::deallocate(Address addressToDelete, std::size_t sizeToDelete){
    // Remove record from block
    memset((char *)addressToDelete.blockAddress + addressToDelete.offset, '\0', sizeToDelete);

    // Update actual size used
    actualSizeUsed -= sizeToDelete;
    
    // // If block is empty, just remove the size of the block (but don't deallocate block!).
    // // Create a new test block full of NULL to test against the actual block to see if it's empty.
    // unsigned char testBlock[blockSize];
    // memset(testBlock, '\0', blockSize);

    // // Block is empty, remove size of block.
    // if (memcmp(testBlock, addressToDelete.blockAddress, blockSize) == 0)
    // {
    //   sizeUsed -= blockSize;
    //   allocated--;
    // }

    return true;
}

// Give a block address, offset and size, returns the data there.
void *MemoryPool::loadFromDisk(Address address, std::size_t size) {
    void *dataAddress = operator new(size);
    memcpy(dataAddress, (char *)address.blockAddress + address.offset, size);

    // Update blocks accessed
    blocksAccessed++;

    return dataAddress;
}

// Save records into disk. Return disk address
Address MemoryPool::saveToDisk(void *itemAddress, std::size_t size) {
    Address diskAddress = allocate(size);
    memcpy((char *)diskAddress.blockAddress + diskAddress.offset, itemAddress, size);

    // Update blocks accessed
    blocksAccessed++;

    return diskAddress;
}

// Update data in disk if I have already saved it before.
Address MemoryPool::saveToDisk(void *itemAddress, std::size_t size, Address diskAddress)
{
  memcpy((char *)diskAddress.blockAddress + diskAddress.offset, itemAddress, size);

  // Update blocks accessed
  blocksAccessed++;

  return diskAddress;
}

MemoryPool::~MemoryPool(){};