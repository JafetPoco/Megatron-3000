#include "freeBlockMan.h"
#include <iostream>

void FreeBlockManager::persist() {
  file.seekp(0);
  file.write(bitmap.data(), bitmap.size());
  file.flush();
}

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

void FreeBlockManager::freeBlock(BlockID id) {
  if (id >= 0 && (std::size_t)id < totalBlocks) {
    bitmap[id] = '0';
    persist();
  }
}

bool FreeBlockManager::isBlockFree(BlockID id) const {
  return (id >= 0 && (std::size_t)id < totalBlocks) ? (bitmap[id] == '0') : false;
}

std::size_t FreeBlockManager::freeBlockCount() const {
  std::size_t count = 0;
  for (char c : bitmap) {
    if (c == '0')
      ++count;
  }
  return count;
}
