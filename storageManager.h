#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "diskController.h"

typedef struct FileHandle {
  char *fileName;
  int totalNumBlocks;
  int curBlockPos;
} FileHandle;

typedef char* SM_PageHandle;

class StorageManager {
  private:
    DiskController* diskController;
    int sizeBlock;
  public:
    StorageManager();
    void initDisckController(bool newDisk);
    int createBlockFile(char *fileName); //Crea un ainthivo con un solo bloque;
    int openBlockFile();
    int closePageFile (FileHandle *fHandle);
    int destroyPageFile (char *fileName);
};

#endif //STORAGE_MANAGER_H