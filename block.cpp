#include "block.h"
#include <cstring>
#include <iostream>
#include <filesystem>
#include <sstream>
#include "globals.h"

#define DEBUG
#define VERBOSE

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
    header = "";
    return;
  }
  size_t sector = id*4;
  data = "";
  header = "";
  for (size_t i = 0; i<disk->info().blockLength; i++) {
    size_t sector_offs = sector + i;
    string sectorData = disk->readSector(sector_offs);
#ifdef DEBUG
    std::cerr << "BLOCK: Leyendo sector " << sector_offs << ", data: " << sectorData << std::endl;
#endif
    data += sectorData;
  }
  if(data != ""){
    header = data.substr(0, 8);
    data = data.substr(8);
#ifdef DEBUG
    std::cerr << "BLOCK: Header: " << header << ", Data: " << data << std::endl;
#endif
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
  #ifdef VERBOSE
  std::cerr<<"BLOCK: datos a escribir: "<<data<<std::endl;
  #endif
  if (!disk || !disk->isDiskOpen()) {
#ifdef VERBOSE
    std::cerr << "BLOCK: El disco no se inició\n";
#endif
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
