#ifndef DISK_H
#define DISK_H

#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct pos {
  size_t platter, surface, track, sector;
};

typedef struct __disk_info__ {
  std::string diskName;
  size_t platters, tracks, sectors, sectorSize, blockLength;
} DiskInfo;

class Disk {
private:
  std::string diskRoot;
  size_t platters, tracks, sectors, sectorSize, capacity, freeSpace, totalSectors, blockLength;
  void readMetadata();
  void writeMetadata();
public:
  //Crear
  Disk(std::string diskName, size_t platters, size_t tracksPerSurface, size_t sectorsPerTrack, size_t sectorSize, size_t sectorsPerBlock);
  //leer
  Disk(std::string nameDisk);

  // Formatear el disco (accion destructiva)
  void format();

  //operaciones E/S
  std::string readSector(size_t sector_id);
  void writeSector(size_t sector_id, std::string data);

  const DiskInfo info() const {
    return {
      diskRoot,
      platters,
      tracks,
      sectors,
      sectorSize,
      blockLength
    };
  };

  //Sector numerado en cilindros
  std::fstream openNthSector(size_t sector_id) const;
  pos getNthSector(size_t sector_id) const;
  size_t getNthSector(pos sector_pos)const ;
  size_t getSectorFreeSpace(size_t sector_id) const;
  std::string getSectorPath(size_t sector_id) const;

  //operaciones debug
  void printDiskInfo();
  void printDiskTree();
  void printSectorCont(size_t sector_id);
  bool doesSectorExist(pos sector_pos) const;
  bool doesSectorExist(size_t sector_id)const;
  void printSectorPos(size_t sectorId);

  //assertions
  void assertOnRange(pos sector_pos);

  size_t getTotalSectors() const {
    return totalSectors;
  }
  std::string getDiskRoot() const {
    return diskRoot;
  }

  bool isDiskOpen();
  void updateMetadata();
};
#endif // DISK_H