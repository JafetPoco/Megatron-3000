#include<iostream>
#include<fstream>
#include "globals.h"
#include "block.h"
#include "file.h"
#include "freeBlockMan.h"
#include "tableFiles.h"
#include "bufPool.h"

using namespace std;

Disk *disk = nullptr;
TableFiles *tableFile = nullptr;
FreeBlockManager *freeBlock = nullptr;
BufPool *bufferPool = nullptr;

int main(){
  disk = new Disk("Megatron");
  //disk = new Disk("Megatron", 2, 4, 4, 512, 4);
  freeBlock = new FreeBlockManager("Megatron", 40);
  tableFile = new TableFiles(disk);
  bufferPool = new BufPool(2);

  File archivo("Jafet");

  archivo.write("Hola como estass!!!");

  bufferPool->clearBuffer();




  return 0;
}
