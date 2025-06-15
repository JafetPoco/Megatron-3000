#include "block.h"
#include <cstring>
#include <iostream>
#include <filesystem>
#include <sstream>
#include "globals.h"

namespace fs = std::filesystem;

Block::Block() {}

Block::Block(BlockID id) : id(id) {
  openBlock(id);
}

void Block::openBlock(BlockID id) { 
  this->id = id;
  if (!disk || !disk->isDiskOpen()) {
    std::cerr << "BLOCK: El disco no se inicio\n" ;
    data = "";
    return;
  }
  size_t sector = id*4;
  data = "";
  for (size_t i = 0; i<disk->info().blockLength; i++) {
    size_t sector_offs = sector + i;
    string sectorData = disk->readSector(sector_offs);
    // cout<<sectorData;
    data += sectorData;
  }
}

void Block::saveBlock() {
  std::cout<<"BLOCK: datos a escribir: "<<data<<std::endl;
  if (!disk || !disk->isDiskOpen()) {
    std::cerr << "BLOCK: El disco no se inició\n";
    data = "";
    return;
  }

  size_t sectorSize = disk->info().sectorSize;
  size_t blockLength = disk->info().blockLength;
  size_t totalCapacity = sectorSize * blockLength;

  if (data.size() > totalCapacity) {
    std::cerr << "BLOCK: WARN: La data a escribir excede la capacidad de bloque, se truncará.\n";
  }

  size_t sectorStart = id * blockLength;

  for (size_t i = 0; i < blockLength; i++) {
    size_t offset = i * sectorSize;
    string sectorData;

    if (offset < data.size()) {
      // Copia los datos si quedan datos disponibles
      sectorData = data.substr(offset, sectorSize);
      if (sectorData.size() < sectorSize) {
        sectorData.append(sectorSize - sectorData.size(), '\0');  // Padding
      }
    } else {
      // Ya no hay más datos, escribir sector limpio
      sectorData = string(sectorSize, '\0');
    }

    std::cout<<"BLOCK: sectorData -> block: "<<sectorData<<std::endl;
    disk->writeSector(sectorStart + i, sectorData);
  }
}
