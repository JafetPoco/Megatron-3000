//#define DEBUG
//#define VERBOSE
#include "freeBlockMan.h"
#include <iostream>

/*
Escribe el mapa de bits en el disco
Autor: Berly Dueñas
*/

void FreeBlockManager::persist() {
  file.seekp(0);
#ifdef DEBUG
  std::cerr << "FBM: Persistiendo bitmap en disco.\n";
#endif
  file.write(bitmap.data(), bitmap.size());
  file.flush();
}

/*
INPUT: Nombre del disco y numero de bloques del disco
Abre el bit map del disco (si es que existe), de lo contrario
crea uno nuevo
Autor: Berly Dueñas
*/

FreeBlockManager::FreeBlockManager(std::string fname, std::size_t numBlocks)
    : filename(fname), totalBlocks(numBlocks) {

  bitmap.resize(totalBlocks, '0');
  bitmap[0] = '1';

  std::string path = filename + "/platter_0/surface_1/track_0/sector_0";

  if (std::filesystem::exists(path)) {
    file.open(path, std::ios::in | std::ios::out);
    if (!file.is_open()) {
#ifdef VERBOSE
      std::cerr << "FBM: No se pudo abrir el bitmap.\n";
#endif
      return;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0);

    if (fileSize == totalBlocks) {
#ifdef VERBOSE
      std::cerr << "FBM: Bitmap existente, leyendo...\n";
#endif
      file.read(bitmap.data(), bitmap.size());
    } else {
#ifdef VERBOSE
      std::cerr << "FBM: Bitmap inválido, reescribiendo...\n";
#endif
      file.close();
      file.open(path, std::ios::out | std::ios::trunc);
      file.write(bitmap.data(), bitmap.size());
      file.close();
      file.open(path, std::ios::in | std::ios::out);
    }
  } else {
#ifdef VERBOSE
    std::cerr << "FBM: No existe, creando nuevo bitmap...\n";
#endif
    std::ofstream out(path);
    if (!out.is_open()) {
#ifdef VERBOSE
      std::cerr<<"FBM: fallo escritura de bitmap\n";
      std::cerr<<path;
#endif
    }
    out.write(bitmap.data(), bitmap.size());
    out.close();
    file.open(path, std::ios::in | std::ios::out);
  }
}

/*
OUTPUT: Id de un bloque
Busca en el bit map el primer bloque vacio y
devuelve su Id
Autor: Berly Dueñas
*/

BlockID FreeBlockManager::allocateBlock() {
  for (BlockID id = 0; id < (BlockID)totalBlocks; ++id) {
    if (bitmap[id] == '0') {
#ifdef DEBUG
      std::cerr << "FBM: Asignando bloque libre: " << id << std::endl;
#endif
      bitmap[id] = '1';
      persist();
      return id;
    }
  }
#ifdef VERBOSE
  std::cerr << "FBM: No hay bloques libres disponibles.\n";
#endif
  return -1;
}

/*
INPUT: ID del bloque
Cambia el valor a 0 en el bit map,
liberando el bloque para su uso en el futuro
Autor: Berly Dueñas
*/

void FreeBlockManager::freeBlock(BlockID id) {
  if (id >= 0 && (std::size_t)id < totalBlocks) {
#ifdef DEBUG
    std::cerr << "FBM: Liberando bloque: " << id << std::endl;
#endif
    bitmap[id] = '0';
    persist();
  }
}

/*
INPUT: Id del bloque
OUTPUT: booleano
Me indica si el bloque con el id indicado
está ocupado o vacio
Autor: Berly Dueñas
*/

bool FreeBlockManager::isBlockFree(BlockID id) const {
  bool free = (id >= 0 && (std::size_t)id < totalBlocks) ? (bitmap[id] == '0') : false;
#ifdef DEBUG
  std::cerr << "FBM: isBlockFree(" << id << ") = " << free << std::endl;
#endif
  return free;
}

/*
OUTPUT: Numero de bloques vacios
Cuenta cuantos bloques vacios existen
Autor: Berly Dueñas
*/

std::size_t FreeBlockManager::freeBlockCount() const {
  std::size_t count = 0;
  for (char c : bitmap) {
    if (c == '0')
      ++count;
  }
#ifdef DEBUG
  std::cerr << "FBM: Bloques libres: " << count << std::endl;
#endif
  return count;
}
