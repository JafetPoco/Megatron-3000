#ifndef DISCO_H
#define DISCO_H

#include "dberror.h"
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class Disk {
private:
  std::string diskName;
  int platters;
  int tracksPerSurface;
  int sectorsPerTrack;
  int sectorSize;

  long availableSpace;
  long capacity;
  
  void writeMetadata();
  void readMetadata();
  // Métodos auxiliares
  //RC ensureDirectoryStructure();
  //std::string getSectorPath(int platter, int track, int sector) const;
  //RC validateAddress(int platter, int track, int sector) const;

public:
  // Constructor con parámetros configurables
  Disk(const std::string& diskName, int platters, int tracksPerSurface, int sectorsPerTrack, int sectorSize);
  Disk();
  
  // Operaciones básicas de disco
  void readSector(int platter, int track, int sector, char* buffer);
  void writeSector(int platter, int track, int sector, const char* data);
  void format();
  void getInfo();

  int getPlatters(){ return platters; }
  int getTracksPerSurface(){ return tracksPerSurface; }
  int getSectorPerTrack(){ return sectorsPerTrack; }
  int getSectorSize(){ return sectorSize; }

  //std::string getDiskPath() const { return diskName; }
};

#endif // DISCO_H