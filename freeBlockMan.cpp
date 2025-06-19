#include "freeBlockMan.h"
#include <iostream>

/*
Escribe el mapa de bits en el disco
Autor: Berly Dueñas
*/

void FreeBlockManager::persist() {
  file.seekp(0);
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
      std::cerr << "No se pudo abrir el bitmap.\n";
      return;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0);

    if (fileSize == totalBlocks) {
      std::cout << "Bitmap existente, leyendo...\n";
      file.read(bitmap.data(), bitmap.size());
    } else {
      std::cout << "Bitmap inválido, reescribiendo...\n";
      file.close();
      file.open(path, std::ios::out | std::ios::trunc);
      file.write(bitmap.data(), bitmap.size());
      file.close();
      file.open(path, std::ios::in | std::ios::out);
    }
  } else {
    std::cout << "No existe, creando nuevo bitmap...\n";
    std::ofstream out(path);
    if (!out.is_open()) {
      std::cerr<<"FBM: fallo escritura de bitmap\n";
      std::cerr<<path;
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
      bitmap[id] = '1';
      persist();
      return id;
    }
  }
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
  return (id >= 0 && (std::size_t)id < totalBlocks) ? (bitmap[id] == '0') : false;
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
  return count;
}
