#ifndef FILE_H
#define FILE_H

#include <cstddef>
#include <vector>
#include <unordered_set>
#include "block.h"
#include "globals.h"

class TableFiles;
class FreeBlockManager;

class File{
private:
  std::string nameFile;
  std::unordered_set<ssize_t> orderBlock;
  std::vector<ssize_t> orderBlockList;
  size_t currentByte;
  void addBlock(size_t id);
  void updateOrderBlocks(size_t blockPos);
public:

  File(std::string name);
  static void setBufferPool(BufPool* bp){ bufferPool = bp; }

  void open(size_t position);
  void close();
  void seek(size_t numByte);

  void write(std::string data);
  std::string read(size_t size);

  size_t getCurrentByte() const { return currentByte; }
};

#endif 