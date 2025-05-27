#include<iostream>
#include<filesystem>
#include<fstream>
#include <unistd.h> 
#include "disk.h"

using namespace std;

Disk::Disk(const string& diskName, int platters, int tracksPerSurface, int sectorsPerTrack, int sectorSize)
  : diskName(diskName), platters(platters), tracksPerSurface(tracksPerSurface), sectorsPerTrack(sectorsPerTrack), sectorSize(sectorSize){
    capacity = platters*tracksPerSurface*sectorsPerTrack*sectorSize;
  }

Disk::Disk(){
  diskName = "Megatron";
  platters = 4;
  tracksPerSurface = 32;
  sectorsPerTrack = 32;
  sectorSize = 512;
  capacity = platters*tracksPerSurface*sectorsPerTrack*sectorSize;
  readMetadata();
}

void Disk::writeMetadata(){
  if(sizeof(long) > sectorSize){
    cerr<<"Los sectores son muy pequeños"<<endl;
    return;
  }

  availableSpace = capacity - sizeof(long);
  string path = diskName + "/platter_0/surface_0/track_0/sector_0";
  ofstream meta(path, ios::binary);
  meta.write(reinterpret_cast<const char*>(&availableSpace), sizeof(long));
  meta.close();
  printf("Capacidad del disco:\t%ld B\nEspacio disponible:\t%ld B\n", capacity, availableSpace);
}

void Disk::readMetadata(){
  string path = diskName + "/platter_0/surface_0/track_0/sector_0";
  ifstream meta(path, ios::binary);
  if(!meta){
    cerr<<"No existe archivo"<<endl;
    return;
  }
  meta.read(reinterpret_cast<char*>(&availableSpace), sizeof(long));
  meta.close();
  printf("Capacidad del disco:\t%ld B\nEspacio disponible:\t%ld B\n", capacity, availableSpace);
}

void Disk::format(){
  fs::remove_all(diskName);
  fs::create_directories(diskName);

  for(int i=0; i<platters; i++){
    string platterPath = diskName + "/platter_" + to_string(i);
    fs::create_directories(platterPath);
    for(int l=0; l<2; l++){
      string surfacePath = platterPath + "/surface_" + to_string(l);
      fs::create_directories(surfacePath);
      for(int j=0; j<tracksPerSurface; j++){
        string trackPath = surfacePath + "/track_" + to_string(j);
        fs::create_directories(trackPath);
        for(int k=0; k<sectorsPerTrack; k++){
          ofstream file(trackPath+"/sector"+to_string(k), ios::binary);
          file.seekp(sectorSize-1);
          file.put('\0');
          file.close();
        }
      }
    }
  }
  writeMetadata();
  return;
}

void Disk::getInfo(){
  long total = platters*tracksPerSurface*sectorsPerTrack*sectorSize;
  printf("Capacidad del disco %ld bytes", total);
}

void Disk::readSector(int platter, int track, int sector, char* buffer){
  string path = diskName+"/platter_"+to_string(platter)+"/track_"+to_string(track)+"/sector_"+to_string(sector);
  ifstream file(path, ios::binary);
  if (!file) {
    cerr<<"No se pudo abrir sector para lectura"<<endl;
    return;
  }

  file.read(buffer, sectorSize);
  file.close();
}

void Disk::writeSector(int platter, int track, int sector, const char* data){
  string path = diskName+"/platter_"+to_string(platter)+"/track_"+to_string(track)+"/sector_"+to_string(sector);
  ofstream file(path, ios::binary);
  if (!file) {
    cerr<<"No se pudo abrir sector para escritura"<<endl;
    return;
  }

  file.write(data, sectorSize);
  file.close();
}