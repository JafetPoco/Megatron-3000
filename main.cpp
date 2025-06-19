#include<iostream>
#include<fstream>
#include "globals.h"
#include "block.h"
#include "file.h"
#include "freeBlockMan.h"
#include "tableFiles.h"
#include "bufPool.h"
#include "recordManager.h"
#include "menu.h"

using namespace std;

int main(){
  menu();
  bufferPool->clearBuffer();
  return 0;
}
