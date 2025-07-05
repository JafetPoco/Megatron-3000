#include "disk.h"
#include "block.h"
#include "freeBlockMan.h"
#include "tableFiles.h"
#include "globals.h"
#include <iostream>
using namespace std;

Disk* disk = nullptr;
FreeBlockManager* freeBlock=nullptr;
TableFiles* tableFile=nullptr;

int main() {
  disk = new Disk("Megatron", 2,2,4,512,8);
  freeBlock = new FreeBlockManager("Megatron", 256);
  tableFile = new TableFiles(); //carga si existe el disco
  Block page(0);
  
  tableFile->addFile("test1");
  tableFile->addFile("test2");
  tableFile->addFile("test3");
  tableFile->addFile("test4");
  tableFile->addFile("test5");
  tableFile->addFile("test6");
  tableFile->showTable();

  page.openBlock(0);
  cout<<page.getData()<<endl;
}

/****************************************************/
/****************************************************/
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
