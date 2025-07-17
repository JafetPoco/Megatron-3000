#include "file.h"
#include "block.h"
#include "bufPool.h"
#include "globals.h"
#include <iostream>
#include <string>

ssize_t File::capacity = 0;

File::File() : fileName(""), firstBlock(-1), currentBlockID(-1), mode('r'), currentBlock(nullptr) {}

File::File(std::string fileName, char mode) : currentBlock(nullptr) {
  open(fileName, mode);
}

bool File::open(std::string fileName, char mode) {
  this->fileName = fileName;
  this->mode = mode;
  BlockID firstID = tableFile->findFile(fileName);
  if (firstID == -1) {
    firstID = tableFile->addFile(fileName);
  }
  firstBlock = firstID;
  currentBlockID = firstID;
  currentBlock = nullptr;
  if (bufferPool) {
    currentBlock = &bufferPool->requestPage(firstBlock, mode);
    bufferPool->clearBuffer();
  }
  return currentBlock != nullptr;
}

bool File::close() {
  fileName = "";
  currentBlock = nullptr;
  currentBlockID = -1;
  return true;
}

std::string& File::read() const {
  if (!currentBlock) {
    std::cerr<<"FILE:No se cargo un archivo\n";
    exit(1);
  };
  std::cerr<<"FILE: retornando currentBlock para read(): "<<*currentBlock<<'\n';
  return *currentBlock;
}

// string File::readAll() {
//   if (!isOpen() && !currentBlock) {
//     std::cerr<<"FILE: no se abrio un archivo. error\n";
//     return "";
//   }

//   auto cap = getCapacity();
//   string all=currentBlock->substr(4);
//   while (getNext() != 0x00) {

//   }
// }

bool File::write(std::string input) {
  if (!isOpen() || !currentBlock) return false;

  ssize_t cap = getCapacity();
  size_t written = 0;
  BlockID thisBlockID = currentBlockID;
  std::string* blockData = currentBlock;

  while (written < input.size()) {
    if (!blockData) return false;

    // Asegura que el bloque tenga al menos 4 bytes para el header
    if (blockData->size() < 4) blockData->resize(4, '0');

    size_t toWrite = std::min((size_t)cap, input.size() - written);

    if (blockData->size() < 4 + toWrite)
      blockData->resize(4 + toWrite, '_');

    // Escribe los datos después de los 4 bytes del encabezado
    blockData->replace(4, toWrite, input.substr(written, toWrite));

    // Por defecto, apunta al siguiente como "0000"
    blockData->replace(0, 4, "0000");

    written += toWrite;

    if (written < input.size()) {
      // Necesita un nuevo bloque
      BlockID newBlock = freeBlock->allocateBlock();

      // Apunta al nuevo bloque
      char header[5];
      snprintf(header, sizeof(header), "%04zd", newBlock);
      blockData->replace(0, 4, header);

      // No se libera el bloque, BufPool se encarga del manejo

      // Pide nuevo bloque al buffer
      currentBlockID = newBlock;
      currentBlock = &bufferPool->requestPage(currentBlockID, 'w');
      blockData = currentBlock;

      if (blockData->size() < 4) blockData->resize(4, '0');
      blockData->replace(0, 4, "0000");
    }
  }

  return true;
}

bool File::isOpen() const {
  return !fileName.empty() && currentBlock != nullptr;
}

BlockID File::getNext() const {
  if (!isOpen() || !currentBlock) return 0;
  std::string& data = *currentBlock;
  if (data.size() < 4) return 0;
  return std::stoul(data.substr(0, 4));
}

BlockID File::getCurrent() const {
  if (!isOpen() || !currentBlock) return 0;
  return currentBlockID;
}

bool File::nextBlock() {
  if (!isOpen()) return false;
  string& data = *currentBlock;
  if (data.size() < 4) return false;
  BlockID nextID = getNext();
  if (nextID == 0) return false;
  currentBlockID = nextID;
  currentBlock = &bufferPool->requestPage(currentBlockID, mode);
  return true;
}

ssize_t File::getCapacity() const {
  if (capacity > 0) return capacity;
  if (blockCapacity > 0) return blockCapacity - 4;
  return 512 - 4;
}

