#include "bufPool.h"
#include "hash.h"
#include "file.h"
#include "disk.h"
#include "block.h"
#include "freeBlockMan.h"
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

  Directory d(2, 4);
  d.insert(1, 10);
  d.insert(2, 20);

  bufferPool->clearBuffer();
  main_cli();
}

/****************************************************/
/*************************TESTS**********************/
/****************************************************/

bool createDisk() {
  disk =new Disk("Megatron", 1,2,4,512,8);
  disk->printDiskInfo();
  // for (int i = 0; i<disk.getTotalSectors(); i++) {
  //   cout<<disk.getSectorPath(i)<<'\n';
  // }
  cout<<"Total sectors: "<<disk->getTotalSectors()<<'\n';
  return true;
}
bool testDisk() {

  createDisk();
  if (!disk->isDiskOpen()) {
    return false;
  }
  cerr<<"ROOT DISK: "<<disk->getDiskRoot()<<'\n';
  disk->printDiskInfo();
  disk->printDiskTree();
  cerr<<"Contenido sector 0\n";
  disk->printSectorCont(0);
  return true;
}

bool testTableFiles() {
  tableFile = new TableFiles();
  return true;
}

void testBuffer() {
  bufferPool = new LRU(4);
  bufferPool->requestPage(0, 'r');
  bufferPool->requestPage(1, 'r');
  bufferPool->requestPage(2, 'r');
  bufferPool->requestPage(3, 'r');
  string& test = bufferPool->requestPage(0, 'r');
  cout<<"Data in block 0: "<<test<<endl;
}
