#include "bufPool.h"
#include "hash.h"
#include "file.h"
#include "disk.h"
#include "block.h"
#include "freeBlockMan.h"
#include "recordManager.h"
#include "schema.h"
#include "tableFiles.h"
#include "globals.h"
#include <iostream>
#include "cli.h"
using namespace std;

Disk* disk = nullptr;
FreeBlockManager* freeBlock=nullptr;
TableFiles* tableFile=nullptr;
BufPool* bufferPool = nullptr;
SchemaManager* schemas= nullptr;
size_t blockCapacity;

int main() {
  // disk = new Disk("Megatron", 8,8,8,512,4); 
  disk = new Disk("Megatron");
  blockCapacity = disk->info().sectorSize * disk->info().blockLength;
  File::set_capacity(blockCapacity);
  //cerr<<disk->getTotalSectors()<<endl;
  //disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", disk->getTotalSectors()/disk->info().blockLength);
  tableFile = new TableFiles(); //carga si existe el disco
  bufferPool = new Clock(5);
  schemas = new SchemaManager;

  bufferPool->clearBuffer();
  // main_cli();
  RecordManager* rm = new RecordManagerFixed("titanic");
  CSVProcessor csv("titanic.csv");
  // csv.process();
  auto test =csv.getData();
}
