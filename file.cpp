#include "file.h"
#include "block.h"
#include "bufPool.h"
#include "globals.h"
#include <iostream>
#include <string>

#define DEBUG

string blockidTOString(BlockID id) {
  std::ostringstream oss;
  oss << std::setw(4) << std::setfill('0') << id;
  return oss.str();
}

ssize_t File::capacity = 0;

File::File() : fileName(""), mode('r'), currentBlockID(-1), payload("") {}

File::File(std::string fileName, char mode) {
  open(fileName, mode);
}

bool File::open(std::string fileName, char mode) {
  BlockID firstID = tableFile->findFile(fileName);
  if (firstID == -1) {
    firstID = tableFile->addFile(fileName);
    if (firstID == -1) {
      std::cerr<<"No se pudo alojar un bloque\n";
      return false;
    }
  }
  this->fileName = fileName;
  this->mode = mode;
  blocks.push_back(firstID);
  currentBlockID = firstID;

  string& currentBlock = bufferPool->requestPage(currentBlockID, mode);
  payload = currentBlock.substr(4); // Skip the first 4 bytes (block ID)
  return true;
}

bool File::close() {
  if (!isOpen()) return false;
  // Save the current block data before closing
  string& currentBlock = bufferPool->requestPage(currentBlockID, mode);
  currentBlock = blockidTOString(getNext()) + payload; // Save the block ID and payload

  string& clone = bufferPool->requestPage(currentBlockID, mode);
#ifdef DEBUG
  std::cerr<<"payload: " << payload << std::endl;
  std::cerr << "FILE: CLOSE() Guardando datos en el bloque " << currentBlockID << ": " << currentBlock << std::endl;
std::cerr<<"clone string "<< clone << std::endl;
#endif

  fileName = "";
  blocks.clear();
  currentBlockID = -1;
  payload = "";
  return true;
}

std::string& File::accessBlock() {
  if (!isOpen()) {
    std::cerr<<"FILE: no se abrio un archivo. error\n";
    throw std::runtime_error("File not open");
  }
  if (currentBlockID == -1) {
    std::cerr<<"FILE: no hay bloque actual. error\n";
    throw std::runtime_error("No current block");
  }
  return payload;
}

bool File::isOpen() const {
  return !fileName.empty() && currentBlockID != -1;
}

BlockID File::getNext() const {
  if (!isOpen() || currentBlockID == -1) return 0;
  string& data = bufferPool->requestPage(currentBlockID, mode);
  if (data.size() < 4) return 0;
  return std::stoul(data.substr(0, 4));
}

BlockID File::getCurrent() const {
  if (!isOpen() || currentBlockID == -1) return 0;
  return currentBlockID;
}

bool File::nextBlock() {
  if (!isOpen()) return false;
  string& data = bufferPool->requestPage(currentBlockID, mode);
  if (data.size() < 4) return false;
  BlockID nextID = getNext();
  if (nextID == 0) return false;

  //save data on the current block before moving to the next
  data = blockidTOString(nextID) + payload;
#ifdef DEBUG
  std::cerr << "FILE: nextBlock() Guardando datos en el bloque " << currentBlockID << ": " << data << std::endl;
#endif

  blocks.push_back(nextID);
  currentBlockID = nextID;
  string& nextBlock = bufferPool->requestPage(currentBlockID, mode);
  payload = nextBlock.substr(4); // Skip the first 4 bytes (block ID)
  return true;
}

bool File::addBlock() {
  if (!isOpen()) return false;
  if (getNext() != 0) return false;
  BlockID newBlockID = freeBlock->allocateBlock();
  std::string& data = bufferPool->requestPage(currentBlockID, mode);
  data = blockidTOString(newBlockID) + payload;
  blocks.push_back(newBlockID);
  currentBlockID = newBlockID;
  return true;
}

ssize_t File::getCapacity() const {
  if (capacity > 0) return capacity-4;
  return 512 - 4;
}

