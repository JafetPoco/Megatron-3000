#include "block.h"
#include <cstring>
#include <iostream>
#include <filesystem>
#include <ostream>
#include <sstream>
#include "globals.h"

// #define DEBUG
// #define VERBOSE

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
#ifdef VERBOSE
    std::cerr << "BLOCK: El disco no se inicio\n" ;
#endif
    data = "";
    return;
  }
  size_t sector = id*disk->info().blockLength;
  data = "";
  for (size_t i = 0; i<disk->info().blockLength; i++) {
    size_t sector_offs = sector + i;
    string sectorData = disk->readSector(sector_offs);
#ifdef DEBUG
    std::cerr << "BLOCK: Leyendo sector " << sector_offs << ", data: " << sectorData << std::endl;
#endif
    data += sectorData;
  }
}

/*
Guarda la información del bloque en el disco, fracmentandolo
en los sectores correcpondientes
Autor: Berly Dueñas
*/

void Block::saveBlock() {
  #ifdef VERBOSE
  std::cerr<<"BLOCK: datos a escribir: "<<data<<std::endl;
  std::cerr<<"BLOCK: size de datos: "<<data.size()<<std::endl; 
  #endif
  if (!disk || !disk->isDiskOpen()) {
#ifdef VERBOSE
    std::cerr << "BLOCK: El disco no se inició\n";
#endif
    data = "";
    return;
  }

  size_t sectorSize = disk->info().sectorSize;
  size_t blockLength = disk->info().blockLength;
  size_t totalCapacity = sectorSize * blockLength;

  size_t totalLen = strlen(data.c_str());
  string allText = data;

  if (allText.size() > totalCapacity) {
#ifdef DEBUG
    std::cerr << "BLOCK: WARN: La data a escribir excede la capacidad de bloque, se truncará.\n";
#endif
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

    #ifdef DEBUG
    std::cerr<<"BLOCK: sectorData -> block: "<<sectorData<<std::endl;
    #endif
    disk->writeSector(sectorStart + i, sectorData);
  }
}
