#include<iostream>
#include "storageManager.h"

using namespace std;

StorageManager::StorageManager(Disk &disco, int sizeBlock){
  this->disco = &disco;
  this->sizeBlock = sizeBlock;
}
