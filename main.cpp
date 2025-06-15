#include<iostream>
#include<fstream>
#include "globals.h"
#include "disk.h"
#include "block.h"
#include "tableFiles.h"
#include "bufPool.h"

using namespace std;

Disk *disk = nullptr;
TableFiles *tableFile = nullptr;
FreeBlockManager *freeBlock = nullptr;

int main(){
  disk = new Disk("Megatron");
  //disk = new Disk("Megatron", 2, 4, 4, 512, 4);
  freeBlock = new FreeBlockManager("Megatron", 40);

  
  BufPool bp(4);

  bp.requestPage(2, 'r');
  bp.pinFrame(2);
  bp.requestPage(3, 'r');
  bp.pinFrame(3);
  bp.requestPage(5, 'r');
  bp.requestPage(5, 'r');
  bp.pinFrame(5);
  bp.requestPage(6, 'r');
  bp.pinFrame(6);
  bp.requestPage(1, 'r');

  bp.printEstadistic();
  return 0;
}
