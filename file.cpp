#include "file.h"
#include "bufPool.h"
#include "globals.h"
#include <iostream>
#include <string>

ssize_t File::capacity = 0;

File::File() : fileName(""), firstBlock(-1), currentBlock(nullptr), mode('r') {}

File::File(std::string fileName, char mode)
    : fileName(fileName), firstBlock(-1), currentBlock(nullptr), mode(mode) {
  open(fileName, mode);
}

bool File::nextBlock() {
  if (!isOpen())
    return false;
  std::string &data = currentBlock->data->getData();
  if (data.size() < 4)
    return false;
  std::string header = data.substr(0, 4);
  BlockID nextID = std::stoi(header);
  if (nextID == 0)
    return false; // End of list
  buffer->releasePage(currentBlock->data->getID());
  currentBlock = buffer->accessPage(nextID, mode);
  return currentBlock != nullptr;
}

Frame *File::getBlock() { return currentBlock; }

bool File::write(const std::string &input) {
  if (!isOpen() || !currentBlock)
    return false;
  std::string &data = currentBlock->data->getData();
  data = "";
  // Header: 4 bytes for next block pointer
  ssize_t cap = getCapacity();
  size_t written = 0;
  size_t pos = 4;
  while (written < input.size()) {
    size_t toWrite =
        std::min((size_t)cap - (data.size() > 4 ? data.size() - 4 : 0),
                 input.size() - written);
    if (data.size() < 4)
      data = std::string("0000");
    if (data.size() < 4 + toWrite)
      data.resize(4 + toWrite, '_');
    data.replace(pos, toWrite, input.substr(written, toWrite));
    currentBlock->dirty = true;
    written += toWrite;
    if (written < input.size()) {
      // Need a new block
      BlockID newBlock = freeBlock->allocateBlock();
      char header[5];
      snprintf(header, sizeof(header), "%04zd", newBlock);
      data.replace(0, 4, header);
      currentBlock->data->saveBlock();
      buffer->releasePage(currentBlock->data->getID());
      currentBlock = buffer->accessPage(newBlock, 'w');
      if (!currentBlock)
        return false;
      data = "0000";
      pos = 4;
    }
  }
  currentBlock->data->saveBlock();
  return true;
}

ssize_t File::getCapacity() const {
  if (capacity > 0)
    return capacity;
  // Try to get from blockCapacity global
  if (blockCapacity > 0)
    return blockCapacity - 4; // 4 bytes header
  // Fallback: guess
  return 512 - 4;
}

std::string File::getData() const {
  if (!currentBlock)
    return "";
  const std::string &data = currentBlock->data->getData();
  if (data.size() <= 4)
    return "";
  return data.substr(4);
}

bool File::open(string fileName, char mode) {
  BlockID firstID = tableFile->findFile(fileName);
  cout<<"abriendo bloque...\n";
  cout<<"FIRST ID ="<<firstID<<" test:"<<(firstID == -1)<<endl;
  if (firstID == -1) {
    cerr << "FILE: " << fileName << " no existe en disco, creando uno\n";
    firstID = tableFile->addFile(fileName);
  }

  Frame* currentBlock = buffer->accessPage(firstID, mode);

  if (!currentBlock) {
    cerr << "La pagina " << firstID << " no pudo ser leida\n";
    return false;
  }

  this->currentBlock = currentBlock;

  return true;
}

bool File::isOpen() {
  if (fileName.empty()) {
    return false;
  }
  return true;
}

bool File::close() {
  if (currentBlock && currentBlock->dirty) {
    currentBlock->data->saveBlock();
  }
  buffer->releasePage(firstBlock);
  fileName = "";
  return true;
}

size_t File::getNext() {
  if (!isOpen()) {
    cerr << "FILE: Aun no se abrio un archivo\n";
    return 0;
  }
  string &data = currentBlock->data->getData();
  if (data.size() < 4) {
    return 0;
  }
  string header = data.substr(0, 4);
  size_t num = stoi(header);
  return num;
}
