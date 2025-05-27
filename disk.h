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
    int tracksPerPlatter;
    int sectorsPerTrack;
    int sectorSize;
    bool formatted;
    
    // Métodos auxiliares
    //RC ensureDirectoryStructure();
    //std::string getSectorPath(int platter, int track, int sector) const;
    //RC validateAddress(int platter, int track, int sector) const;

public:
    // Constructor con parámetros configurables
    Disk(const std::string& diskName, int platters = 4, int tracksPerPlatter = 32, int sectorsPerTrack = 32, int sectorSize = 512);
    
    // Operaciones básicas de disco
    //RC readSector(int platter, int track, int sector, char* buffer);
    //RC writeSector(int platter, int track, int sector, const char* data);
    
    // Operaciones de mantenimiento
    RC format();
    //RC getInfo(int& totalSectors, int& sectorSize) const;
    void getInfo();
    
    // Estado del disco
    //bool isFormatted() const { return formatted; }
    //std::string getDiskPath() const { return diskName; }
};

#endif // DISCO_H