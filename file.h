#ifndef FILE_H
#define FILE_H

#include <cstddef>
#include <string>
#include <vector>
#include "block.h"
#include "disk.h"
#include "tableFiles.h"
#include "freeBlockMan.h"
#include "bufPool.h"


class File{
private:
  std::string fileName;
  static ssize_t capacity;
  BlockID firstBlock;
  string currentBlock;
  char mode;

public:
  File();
  File(std::string fileName, char mode = 'r');
  size_t getNext();
  static void set_capacity(ssize_t c) { capacity=c; }
  bool open(std::string fileName, char mode='r');
  bool close();
  bool isOpen();
  bool nextBlock();
  string getBlock();
  // New methods
  bool write(const std::string& data);
  ssize_t getCapacity() const;
  std::string getData() const;
};

#endif 
