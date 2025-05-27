#include<iostream>
#include "disk.h"

using namespace std;

int main(){
  Disk disco("Megatron", 4, 32, 32, 512);
  int num = disco.format();
  disco.getInfo();
  return 0;
}
