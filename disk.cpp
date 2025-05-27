#include<iostream>
#include<filesystem>
#include<fstream>
#include <unistd.h> 
#include "disk.h"

using namespace std;

Disk::Disk(const string& diskName, int platters, int tracksPerPlatter, int sectorsPerTrack, int sectorSize)
  : diskName(diskName), platters(platters), tracksPerPlatter(tracksPerPlatter), sectorsPerTrack(sectorsPerTrack), sectorSize(sectorSize), formatted(false) { }

Disk::Disk(){
  diskName = "Megatron";
  platters = 4;
  tracksPerPlatter = 32;
  sectorsPerTrack = 32;
  sectorSize = 512;
}

RC Disk::format(){
  fs::remove_all(diskName);
  fs::create_directories(diskName);

  for(int i=0; i<platters; i++){
    string platterPath = diskName + "/platter_" + to_string(i);
    fs::create_directories(platterPath); 
    for(int j=0; j<tracksPerPlatter; j++){
      string trackPath = platterPath + "/track_" + to_string(j);
      fs::create_directories(trackPath);
      for(int k=0; k<sectorsPerTrack; k++){
        ofstream file(trackPath+"/sector"+to_string(k), ios::binary);
        file.seekp(sectorSize-1);
        file.put('\0');
        file.close();
      }
    }
  }
  return 0;
}

void Disk::getInfo(){
  long total = platters*tracksPerPlatter*sectorsPerTrack*sectorSize;
  printf("Capacidad del disco %ld bytes", total);
}