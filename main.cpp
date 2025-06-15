#include<iostream>
#include<fstream>
#include "globals.h"
#include "disk.h"
#include "block.h"

using namespace std;

Disk *disk = nullptr;

int main(){
  disk = new Disk("Megatron");

  cout<<disk->readSector(0)<<endl;

  Block b;
  b.openBlock(1);
  cout<<"Bloque 1: "<<b.getData()<<endl;



  return 0;
}
