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
  BufPool *bufferPool = nullptr;
  disk = new Disk("Megatron");
  //disk = new Disk("Megatron", 2, 4, 4, 512, 4);
  freeBlock = new FreeBlockManager("Megatron", 40);

  
  bufferPool = new BufPool(2);

  std::string *st = &bufferPool->requestPage(2, 'r');
  cout<<*st<<endl;

  std::string *st1 = &bufferPool->requestPage(2, 'w');
  *st1 = "Nuevo";

  std::string *st2 = &bufferPool->requestPage(3, 'w');
  *st2 = "pagina3";

  cout<<bufferPool->requestPage(0, 'r')<<endl;
  cout<<bufferPool->requestPage(2, 'r')<<endl;

  //*st = "Hola mundo";
  //cout<<*st<<endl;
  //bufferPool->clearBuffer();

  bufferPool->printEstadistic();
  return 0;
}
