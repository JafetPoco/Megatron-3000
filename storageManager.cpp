#include<iostream>
#include "storageManager.h"

using namespace std;

StorageManager::StorageManager(){
  diskController = new DiskController();
}

void StorageManager::initDisckController(bool newDisk){
  diskController->initDisk(newDisk);
  if(newDisk == true){
    cout<<"Ingrese numero de sectores por Bloque: "; cin>>sizeBlock; 
  } else {
    sizeBlock = 4;
  }
}