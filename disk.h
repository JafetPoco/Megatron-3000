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
  /**
   * @brief Lee la metadata escrita en el disco
   * @author Jafet Poco
   */
  void readMetadata();
  /**
   * @brief Escribe la metadata almacenada en la clase al disco
   * @author Jafet Poco
   */
  void writeMetadata();
  /**
   * @brief Saber si el sector existe dentro de una posicion fisica
   * @param sector_pos posicion fisica en disco
   * @return bool
   * @author Berly Dueñas
   */
  bool doesSectorExist(pos sector_pos) const;
  /**
   * @brief Mapear un sector_id a posicion fisica del disco
   * @param sector_id Id del sector
   * @return pos objeto posicion fisica
   * @author Berly Dueñas
   */
  pos getNthSector(size_t sector_id) const;
  /**
   * @brief Permite abrir un sector con una cuenta vertical
   * @param sector_pos posicion del sector
   * @return size_t id del sector
   * @author Berly Dueñas
   */
  size_t getNthSector(pos sector_pos)const ;
public:
  /**
   * @brief Permite crear un disco con los parametros dados
   * @param diskName Nombre del disco
   * @param platters Número de platos
   * @param tracksPerSurface Pistas por superficie
   * @param sectorsPerTrack Sectores por pista
   * @param sectorSize Tamaño del sector
   * @param sectorsPerBlock Sectores por bloque
   * @author Jafet Poco
   */
  Disk(std::string diskName, size_t platters, size_t tracksPerSurface, size_t sectorsPerTrack, size_t sectorSize, size_t sectorsPerBlock);
  /**
   * @brief Abre un disco ya creado con la ruta de la carpeta que lo contiene
   * @param nameDisk Nombre del disco
   * @author Jafet Poco
   */
  Disk(std::string nameDisk);

  /**
   * @brief Formatea un disco (accion destructiva)
   * @author Jafet Poco
   */
  void format();

  /**
   * @brief Lee la información de un sector
   * @param sector_id ID del Sector
   * @return std::string data del sector
   * @author Jafet Poco
   */
  std::string readSector(size_t sector_id);

  /**
   * @brief Escribe información dentro de un sector
   * @param sector_id ID del Sector
   * @param data Información a escribir
   * @author Jafet Poco
   */
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

  /**
   * @brief Retorna el objeto fstream asociado al sector, si existe
   * @param sector_id Id del sector
   * @return std::fstream
   * @author Berly Dueñas
   */
  std::fstream openNthSector(size_t sector_id) const;
  /**
   * @brief Retorna el espacio libre de un sector
   * @param sector_id Id del sector
   * @return size_t espacio libre
   * @author Berly Dueñas
   */
  size_t getSectorFreeSpace(size_t sector_id) const;
  /**
   * @brief Retorna el path del sector
   * @param sector_id Id del sector
   * @return std::string path del sector
   * @author Berly Dueñas
   */
  std::string getSectorPath(size_t sector_id) const;

  /**
   * @brief Imprime informacion general del disco
   * @author Jafet Poco
   */
  void printDiskInfo();
  /**
   * @brief Imprime el arbol de directorios del disco
   * @author Jafet Poco
   */
  void printDiskTree();
  /**
   * @brief Imprime el contenido de sector
   * @param sector_id Id del sector
   * @author Berly Dueñas
   */
  void printSectorCont(size_t sector_id);
  /**
   * @brief Saber si el sector existe dentro de un id de sector
   * @param sector_id id de disco
   * @return bool
   * @author Berly Dueñas
   */
  bool doesSectorExist(size_t sector_id)const;
  /**
   * @brief Imprime la posicion fisica del disco
   * @param sectorId Id del sector
   * @author Berly Dueñas
   */
  void printSectorPos(size_t sectorId);

  size_t getTotalSectors() const {
    return totalSectors;
  }
  std::string getDiskRoot() const {
    return diskRoot;
  }

  /**
   * @brief Verifica si el disco está abierto
   * @return bool
   * @author Jafet Poco
   */
  bool isDiskOpen();
  /**
   * @brief Actualiza la metadata del disco (almacenamiento usado, etc)
   * @author Jafet Poco
   */
  void updateMetadata();
};
#endif // DISK_H
