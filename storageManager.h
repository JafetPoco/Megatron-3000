#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "disk.h"

typedef struct FileHandle {
  char *fileName;
  int totalNumBlocks;
  int curBlockPos;
} FileHandle;

typedef char* SM_PageHandle;

class StorageManager {
  private:
    Disk* disco;
    int sizeBlock;
  public:
    StorageManager(Disk &disco, int sizeBlock = 4);
    int createBlockFile(char *fileName); //Crea un ainthivo con un solo bloque;
    int openBlockFile();
    int closePageFile (FileHandle *fHandle);
    int destroyPageFile (char *fileName);
};

#endif //STORAGE_MANAGER_H