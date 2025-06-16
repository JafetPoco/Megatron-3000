#include <iostream>
#include <iomanip>
#include <sstream>
#include "file.h"
#include "globals.h"
#include "bufPool.h"
#include "tableFiles.h"
#include "freeBlockMan.h"

File::File(std::string name) : nameFile(name), currentByte(0) {
  size_t position;
  if(tableFile->findFile(name, &position) == true){
    open(position);
    return; 
  }
  
  printf("Creando archivo...\n");
  ssize_t posFreeBlock = freeBlock->allocateBlock();
  tableFile->addFile(name, posFreeBlock);
  tableFile->saveChanges(disk);

  orderBlock.insert(posFreeBlock);
  orderBlockList.push_back(posFreeBlock);

  std::string *header = &bufferPool->requestPage(posFreeBlock, 'w');
  *header = "00000008";
}

void File::open(size_t position){
  bufferPool->requestPage(position, 'r');
  orderBlock.insert(position);
  orderBlockList.push_back(position);
}

void File::updateOrderBlocks(size_t blockPos){
  size_t originalSize = orderBlockList.size();
  for(size_t i = originalSize; i < blockPos; i++){
    std::string header = bufferPool->requestPage(orderBlockList[i], 'r');
    size_t nextBlock = (size_t)stoi(header.substr(0, 4));
    orderBlock.insert(nextBlock);
    orderBlockList.push_back(nextBlock);
  }
}

std::string File::read(size_t size){
  size_t sectorSize = disk->info().sectorSize;
  size_t blockSize = disk->info().blockLength * sectorSize;
  size_t blockPos = currentByte / blockSize;
  size_t dataPos = currentByte % blockSize;
  currentByte += size;

  if(orderBlockList.size() < blockPos){
    updateOrderBlocks(blockPos);
  }
  size_t idBlock = orderBlockList[blockPos];
  std::string data = bufferPool->requestPage(idBlock, 'r');
  return data.substr(dataPos, size);
}

void File::write(std::string data){
  size_t sectorSize = disk->info().sectorSize;
  size_t blockSize = disk->info().blockLength * sectorSize;
  size_t blockPos = currentByte / blockSize;
  size_t dataPos = currentByte % blockSize;
  size_t size = data.length();
  if(dataPos + size > blockSize) {
    std::cerr<<"No hay espacio en el bloque, creando uno nuevo\n";
    size_t newBlockPos = freeBlock->allocateBlock();
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << newBlockPos;
    
    
    std::string *headerLastBlock = &bufferPool->requestPage(orderBlockList.back(), 'w');
    headerLastBlock->replace(0, 4, ss.str());
    
    orderBlock.insert(newBlockPos);
    orderBlockList.push_back(newBlockPos);
    
    std::string *newBlockData = &bufferPool->requestPage(newBlockPos, 'w');
    *newBlockData = "0000";
    ss.str(""); ss.clear();
    ss << std::setfill('0') << std::setw(4) << (size + 8);
    *newBlockData += ss.str();
    *newBlockData += data;
  } else {
    if(orderBlockList.size() < blockPos){
      updateOrderBlocks(blockPos);
    }
    
    std::cout<<"Llego?\n"<<blockPos<<std::endl;
    std::cout<<orderBlockList[blockPos]<<std::endl;
    size_t idBlock = orderBlockList[blockPos];
    std::string *blockData = &bufferPool->requestPage(idBlock, 'w');
    blockData->replace(dataPos, size, data);
  }
}

void File::seek(size_t numByte){
  currentByte = numByte;
}