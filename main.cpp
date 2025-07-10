#include "bufPool.h"
#include "file.h"
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
BufPool* bufferPool = nullptr;
size_t blockCapacity;

void testBuffer();

int main() {
  disk = new Disk("Megatron", 2,2,8,512,4);
  // disk = new Disk("Megatron");
  blockCapacity = disk->info().sectorSize * disk->info().blockLength;
  cerr<<disk->getTotalSectors()<<endl;
  disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", 256);
  tableFile = new TableFiles(); //carga si existe el disco
  bufferPool = new Clock(5);

  File test("test1");
  cout<<"Siguiente de test1: "<<test.getNext()<<"\n";
  cout<<"contenido: "<<test.read()<<endl;
  test.write("hola");

  cout<<"contenido: "<<test.read()<<endl;
  bufferPool->clearBuffer();
  
  // tableFile->addFile("test1");
  // tableFile->addFile("test2");
  // tableFile->addFile("test3");
  // tableFile->addFile("test4");
  // tableFile->addFile("test5");
  // tableFile->addFile("test6");
  // tableFile->addFile("test7");
  // tableFile->addFile("test8");
  // tableFile->addFile("test10");
  // tableFile->addFile("test11");
  // tableFile->addFile("test12");
  // tableFile->addFile("test13");
  // tableFile->addFile("test14");
  // tableFile->addFile("test15");
  // tableFile->addFile("test16");
  // tableFile->addFile("test17");
  // tableFile->addFile("test18");
  // tableFile->addFile("test19");
  // tableFile->addFile("test20");
  // tableFile->addFile("test21");
  // tableFile->addFile("test22");
  // tableFile->addFile("test23");
  // tableFile->addFile("test24");
  // tableFile->addFile("test25");
  // tableFile->addFile("test26");
  // tableFile->showTable();

//   page.openBlock(0);
//   cout<<page.getData()<<endl;
  // testBuffer();
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

void testBuffer() {
  bufferPool = new LRU(4);
  bufferPool->requestPage(0, 'r');
  bufferPool->requestPage(1, 'r');
  bufferPool->requestPage(2, 'r');
  bufferPool->requestPage(3, 'r');
  string& test = bufferPool->requestPage(0, 'r');
  cout<<"Data in block 0: "<<test<<endl;
}
