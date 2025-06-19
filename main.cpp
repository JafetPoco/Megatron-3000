#include<iostream>
#include<fstream>
#include "globals.h"
#include "block.h"
#include "file.h"
#include "freeBlockMan.h"
#include "tableFiles.h"
#include "bufPool.h"
#include "recordManager.h"

using namespace std;

Disk *disk = nullptr;
TableFiles *tableFile = nullptr;
FreeBlockManager *freeBlock = nullptr;
BufPool *bufferPool = nullptr;

int main(){
  // disk = new Disk("Megatron");
  disk = new Disk("Megatron", 2, 4, 4, 512, 4);
  disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", 40);
  tableFile = new TableFiles(disk);
  tableFile->showTable();
  bufferPool = new BufPool(2);

  RecordManager *rm = new RecordManagerVariable;
  rm->readCSV("titanic.csv");
  
  bufferPool->clearBuffer();

  return 0;
}
