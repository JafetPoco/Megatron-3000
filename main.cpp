#include<iostream>
#include<fstream>
#include "globals.h"
#include "disk.h"
#include "block.h"
#include "tableFiles.h"

using namespace std;

Disk *disk = nullptr;
TableFiles *tableFile = nullptr;

int main(){
  disk = new Disk("Megatron");
  //disk = new Disk("Megatron", 2, 4, 4, 512, 4);
  tableFile = new TableFiles;

  tableFile->addFile("Nuevo File", 32);
  
  tableFile->showTable();



  return 0;
}
