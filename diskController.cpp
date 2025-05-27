#include<iostream>
#include "diskController.h"

using namespace std;

bool DiskController::existDisk(){
  return disco != nullptr;
}

void DiskController::initDisk(bool newDisk){
  if(newDisk == true){
    int numPlatters, numTracks, numSectores, sizeSector;
    string name;
    cout<<"Ingrese Nombre del disco: "; cin>>name;
    cout<<"Ingrese número de plato: "; cin>>numPlatters;
    cout<<"Ingrese número de pistas por superficie: "; cin>>numTracks;
    cout<<"Ingrese número de sectores por pista: "; cin>>numSectores;
    cout<<"Ingrese tamaño en bytes del sector: "; cin>>sizeSector;
    disco = new Disk(name, numPlatters, numTracks, numSectores, sizeSector);
    disco->format();
    cout<<"\nDisco creado correctamente...\n";
  } else {
    disco = new Disk();
    cout<<"\nDisco cargado correctamente...\n";
  }
}

string DiskController::getAddress(int numBlock) const {
  int numSurfaces = 2*disco->getPlatters();
  int sectorsPerSurface = disco->getTracksPerSurface() * disco->getSectorPerTrack();
  int numberSector = numBlock*4; //4 es la cantidad de secgtores por bloque;
  
  int surface = numberSector/sectorsPerSurface;
  int track = (numberSector % sectorsPerSurface)/disco->getSectorPerTrack();
  int sector = (numberSector % sectorsPerSurface)%disco->getSectorPerTrack();
  int platter = surface/2;

  cout<<platter<<" - "<<surface<<" - "<<track<<" - "<<sector<<endl;
  return "ok";
}