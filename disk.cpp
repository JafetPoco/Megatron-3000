#include<iostream>
#include<filesystem>
#include<fstream>
#include <unistd.h> 
#include "disk.h"

using namespace std;

Disk::Disk(const string& diskName, int platters, int tracksPerPlatter, int sectorsPerTrack, int sectorSize)
  : diskName(diskName), platters(platters), tracksPerPlatter(tracksPerPlatter), sectorsPerTrack(sectorsPerTrack), sectorSize(sectorSize){
    capacity = platters*tracksPerPlatter*sectorsPerTrack*sectorSize;
  }

Disk::Disk(){
  diskName = "Megatron";
  platters = 4;
  tracksPerPlatter = 32;
  sectorsPerTrack = 32;
  sectorSize = 512;
  capacity = platters*tracksPerPlatter*sectorsPerTrack*sectorSize;
  readMetadata();
}

void Disk::writeMetadata(){
  if(sizeof(long) > sectorSize){
    cerr<<"Los sectores son muy pequeños"<<endl;
    return;
  }

  availableSpace = capacity - sizeof(long);
  string path = diskName + "/platter_0/track_0/sector_0";
  ofstream meta(path, ios::binary);
  meta.write(reinterpret_cast<const char*>(&availableSpace), sizeof(long));
  meta.close();
  printf("Capacidad del disco:\t%ld B\nEspacio disponible:\t%ld B\n", capacity, availableSpace);
}

void Disk::readMetadata(){
  string path = diskName + "/platter_0/track_0/sector_0";
  ifstream meta(path, ios::binary);
  if(!meta){
    cerr<<"No existe archivo"<<endl;
    return;
  }
  meta.read(reinterpret_cast<char*>(&availableSpace), sizeof(long));
  meta.close();
  printf("Capacidad del disco:\t%ld B\nEspacio disponible:\t%ld B\n", capacity, availableSpace);
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
  writeMetadata();
  return 0;
}

void Disk::getInfo(){
  long total = platters*tracksPerPlatter*sectorsPerTrack*sectorSize;
  printf("Capacidad del disco %ld bytes", total);
}

RC Disk::readSector(int platter, int track, int sector, char* buffer){
  string path = diskName+"/platter_"+to_string(platter)+"/track_"+to_string(track)+"/sector_"+to_string(sector);
  ifstream file(path, ios::binary);
  if (!file) {
    cerr<<"No se pudo abrir sector para lectura"<<endl;
    return -1;
  }

  file.read(buffer, sectorSize);
  file.close();

}

RC Disk::writeSector(int platter, int track, int sector, const char* data){
  string path = diskName+"/platter_"+to_string(platter)+"/track_"+to_string(track)+"/sector_"+to_string(sector);
  ofstream file(path, ios::binary);
  if (!file) {
    cerr<<"No se pudo abrir sector para escritura"<<endl;
    return -1;
  }

  file.write(data, sectorSize);
  file.close();
}