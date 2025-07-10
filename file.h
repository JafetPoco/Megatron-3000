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
  BlockID currentBlockID;
  string* currentBlock;
  char mode;

public:
  File();
  File(std::string fileName, char mode = 'r');

  bool open(std::string fileName, char mode='r');
  bool close();
  std::string& read() const;
  string readAll();
  bool write(std::string data);

  bool isOpen() const;

  size_t getNext();
  static void set_capacity(ssize_t c) { capacity=c; }
  bool nextBlock();
  ssize_t getCapacity() const;
  //optional: bool remove();
};

#endif 
