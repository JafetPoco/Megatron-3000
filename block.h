#ifndef BLOCK_H
#define BLOCK_H

#include "disk.h"
#include "globals.h"
#include <cstddef>
#include <string>
#include <vector>

using std::string;
using BlockID = ssize_t;

class Block {
private:
  BlockID id;
  size_t usedCapacity;
  string data;

public:
  Block();
  Block(BlockID id);
  void openBlock(BlockID id);
  void saveBlock();

  string& getData() { return data; }

  // Getters
  BlockID getID() const { return id; }
  size_t getUsedCapacity() const { return usedCapacity; }

};

#endif 
