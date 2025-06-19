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

/*
INPUT: Id del bloque
Carga la información de un bloque existente
Autor: Jafet Poco
*/

void Block::openBlock(BlockID id) { 
  this->id = id;
  if (!disk || !disk->isDiskOpen()) {
    std::cerr << "BLOCK: El disco no se inicio\n" ;
    data = "";
    header = "";
    return;
  }
  size_t sector = id*4;
  data = "";
  header = "";
  for (size_t i = 0; i<disk->info().blockLength; i++) {
    size_t sector_offs = sector + i;
    string sectorData = disk->readSector(sector_offs);
    // cout<<sectorData;
    data += sectorData;
  }
  if(data != ""){
    header = data.substr(0, 8);
    data = data.substr(8);
  } else {
    header = "00000008";
  }

}

/*
Guarda la información del bloque en el disco, fracmentandolo
en los sectores correcpondientes
Autor: Berly Dueñas
*/

void Block::saveBlock() {
  std::cout<<"BLOCK: datos a escribir: "<<data<<std::endl;
  if (!disk || !disk->isDiskOpen()) {
    std::cerr << "BLOCK: El disco no se inició\n";
    data = "";
    header = "";
    return;
  }

  size_t sectorSize = disk->info().sectorSize;
  size_t blockLength = disk->info().blockLength;
  size_t totalCapacity = sectorSize * blockLength;

  size_t totalLen = strlen(data.c_str()) + header.size();
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(4) << (totalLen > totalCapacity ? totalCapacity : totalLen);
  header.replace(4, 4, ss.str());
  string allText = header + data;

  if (allText.size() > totalCapacity) {
    std::cerr << "BLOCK: WARN: La data a escribir excede la capacidad de bloque, se truncará.\n";
  }

  size_t sectorStart = id * blockLength;

  for (size_t i = 0; i < blockLength; i++) {
    size_t offset = i * sectorSize;
    string sectorData;

    if (offset < allText.size()) {
      // Copia los datos si quedan datos disponibles
      sectorData = allText.substr(offset, sectorSize);
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
