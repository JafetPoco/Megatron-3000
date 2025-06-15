#ifndef FREEBLOCKMANAGER_HPP
#define FREEBLOCKMANAGER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using BlockID = ssize_t;

class FreeBlockManager {
private:
  std::fstream file;
  std::string filename;
  std::size_t totalBlocks;
  std::vector<char> bitmap;

  void persist(); 

public:
  FreeBlockManager(std::string fname, std::size_t numBlocks);
  BlockID allocateBlock();
  void freeBlock(BlockID id);
  bool isBlockFree(BlockID id) const;
  size_t freeBlockCount() const;
};

#endif
