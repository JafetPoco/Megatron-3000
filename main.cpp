#include<iostream>
#include<fstream>
#include "globals.h"
#include "disk.h"
#include "block.h"
#include "tableFiles.h"

using namespace std;

Disk *disk = nullptr;
TableFiles *tableFile = nullptr;
FreeBlockManager *freeBlock = nullptr;

int main(){
  disk = new Disk("Megatron");
  //disk = new Disk("Megatron", 2, 4, 4, 512, 4);
  freeBlock = new FreeBlockManager("Megatron", 40);

  freeBlock->allocateBlock();

  cout<<"Bitmap: "<<disk->readSector(1)<<endl;


  return 0;
}
