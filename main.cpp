#include<iostream>
#include<fstream>
#include <string>
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
  disk = new Disk("Megatron", 1, 1, 128, 512, 4);
  disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", 40);
  tableFile = new TableFiles(disk);
  tableFile->showTable();
  bufferPool = new BufPool(3);

  RecordManager *rm = new RecordManagerVariable;

  cout<<"===================adflajdfkljrji%%%T##$$$%$%$%$##$#%%$%$%#\n";
  rm->readCSV("titanic.csv");
  rm->select("titanic");
  

  bufferPool->clearBuffer();

  
  return 0;
}
