#include "disk.h"
#include "globals.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <sstream>

// #define DEBUG 
// #define VERBOSE
using namespace std;

/*
INPUT: Parametros del Disco
Permite crear un disco con los parametros Dados
Autor: Jafet Poco
*/

Disk::Disk(string diskName, size_t platters, size_t tracksPerSurface,
           size_t sectorsPerTrack, size_t sectorSize, size_t sectorsPerBlock)
    : diskRoot(diskName), platters(platters),
      tracks(tracksPerSurface), sectors(sectorsPerTrack),
      sectorSize(sectorSize), blockLength(sectorsPerBlock) {
  capacity = platters * 2 * tracksPerSurface * sectorsPerTrack * sectorSize;
  freeSpace = capacity - 40;
  if (freeSpace < 0) {
#ifdef VERBOSE
    cerr << "DISCO: El disco es demasiado pequeño" << endl;
#endif
  }
  format();
  //  getInfo();
}

/*
* INPUT: @param string Nombre Disco
* Abre un disco ya creado con la ruta de la carpeta que lo contiene
* Autor: Jafet Poco
*/
Disk::Disk(string nameDisk) {
  diskRoot = nameDisk;
  if (!fs::exists(diskRoot) && !fs::is_directory(diskRoot)) {
    return;
  }
  cerr << "DISCO: Cargando disco " << diskRoot << "...\n";
  readMetadata();
}

/*
 * Actualiza la metadata del disco (almacenamiento usado, etc)
 * Autor: Jafet Poco
 */

void Disk::updateMetadata(){
  fstream metadata(diskRoot + "/platter_0/surface_0/track_0/sector_0", ios::in | ios::out | ios::binary);
  if (!metadata.is_open()) {
#ifdef VERBOSE
    cerr << "DISCO: Error: No se pudo abrir el sector Metadata" << endl;
#endif
    return;
  }

  stringstream free;
  free << setfill('0') << setw(10) << freeSpace;
  metadata.seekp(10);
  metadata.write(free.str().c_str(), 10);
  metadata.close();
}

/*
Escribe la metadata almacenada en la clase al disco
Autor: Jafet Poco
*/

void Disk::writeMetadata() {
  totalSectors = platters * 2 * tracks * sectors;
  ofstream metadata(diskRoot + "/platter_0/surface_0/track_0/sector_0");
  if (!metadata.is_open()) {
#ifdef VERBOSE
    cerr << "DISCO: Error: No se pudo abrir el sector Metadata" << endl;
#endif
    return;
  }

  metadata << setfill('0') << setw(10)
           << capacity; // 10 primero digitos son la capacidad del disco
  metadata << setfill('0') << setw(10)
           << freeSpace; // 10 primeros digitos son el espacio libre
  metadata << setfill('0') << setw(4)
           << platters; // 4 siguientes numero de platos
  metadata << setfill('0') << setw(4)
           << tracks; // 4 siguientes pistas por sector
  metadata << setfill('0') << setw(4)
           << sectors; // 4 siguientes sectores por pista
  metadata << setfill('0') << setw(6)
           << sectorSize; // 6 siguientes tamaño del sector
  metadata << setfill('0') << setw(2)
           << blockLength; // 2 siguientes sectores por bloque
#ifdef VERBOSE
  cerr << "DISCO: Metadata escrita correctamente\n";
#endif
}

/*
Al abrir un disco, lee la metadata escrita
Autor: Jafet Poco
*/

void Disk::readMetadata() {
  ifstream metadata(diskRoot + "/platter_0/surface_0/track_0/sector_0");
  if (!metadata.is_open()) {
#ifdef VERBOSE
    cerr << "DISCO: Error: No se pudo abrir el sector Metadata" << endl;
#endif
    return;
  }

  char buffer[11];

  // Lee capacidad
  metadata.read(buffer, 10);
  buffer[10] = '\0';
  capacity = atol(buffer);

  //espacio libre
  metadata.read(buffer, 10);
  buffer[10] = '\0';
  freeSpace = atol(buffer);

  //platos
  metadata.read(buffer, 4);
  buffer[4] = '\0';
  platters = atoi(buffer);

  //pistas
  metadata.read(buffer, 4);
  buffer[4] = '\0';
  tracks = atoi(buffer);

  //sectores
  metadata.read(buffer, 4);
  buffer[4] = '\0';
  sectors = atoi(buffer);

  //tamaño sector
  metadata.read(buffer, 6);
  buffer[6] = '\0';
  sectorSize = atoi(buffer);

  //tamaño de bloque
  metadata.read(buffer, 2);
  buffer[2] = '\0';
  blockLength = atoi(buffer);

  totalSectors = platters * 2 * tracks * sectors;
#ifdef VERBOSE
  cerr << "DISCO: Metadata leida correctamente..." << endl;
#endif
}

/*
Formatea un disco
Autor: Jafet Poco
*/

void Disk::format() {
  fs::remove_all(diskRoot);
  fs::create_directories(diskRoot);
#ifdef VERBOSE
  cerr << "DISCO: Creando disco " << diskRoot << endl;
#endif

  for (size_t i = 0; i < platters; i++) { // platos
    string platterPath = diskRoot + "/platter_" + to_string(i);
    fs::create_directories(platterPath);
    for (size_t l = 0; l < 2; l++) { // superficies
      string surfacePath = platterPath + "/surface_" + to_string(l);
      fs::create_directories(surfacePath);
      for (size_t j = 0; j < tracks; j++) { // pistas
        string trackPath = surfacePath + "/track_" + to_string(j);
        fs::create_directories(trackPath);
        for (size_t k = 0; k < sectors; k++) { // sectores
          ofstream file(trackPath + "/sector_" + to_string(k));
        }
      }
    }
  }
  writeMetadata();
  return;
}

/*
INPUT: size_t ID del Sector
OUTPUT: string data del sector
Lee la información de un sector
Autor: Jafet Poco
*/

string Disk::readSector(size_t sector_id) {
  fstream file = openNthSector(sector_id);
  if (!file.is_open()) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: No se pudo abrir sector para lectura" << endl;
#endif
    return "";
  }
  stringstream buf;
  buf << file.rdbuf();

  string data = buf.str();
  // if (data.size() != 0 && data[data.size()-1])
  //   data.pop_back();
  if (data.size() > sectorSize) {
#ifdef DEBUG
    cout<<"DISCO: WARNING El sector leido ha pasado su capacidad\n";
#endif
  }
  // data = data.substr(0, disk->info().sectorSize);
  return data;
}


/*
INPUT: size_t id del Sector y la data en string que se va a escribir
Escribe información dentro de un sector
Autor: Jafet Poco
*/

void Disk::writeSector(size_t sector_id, std::string data) {
#ifdef VERBOSE
  cerr<<"DISCO: f:writeSector Escribiendo a sector: "<<sector_id<<", content: "<<data<<endl;
#endif
  string filePath = getSectorPath(sector_id);
#ifdef DEBUG
  cerr<<"DISCO: path sector: "<<sector_id<<", "<<filePath<<endl;
#endif
  //size_t sizeSectorInit = fs::file_size(filePath);
  ofstream file(filePath, ios::trunc);
  if (!file.is_open()) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: No se pudo abrir sector para escritura" << endl;
#endif
    return;
  }

  if (data.size() > sectorSize) {
#ifdef DEBUG
    cerr<<"DISCO: ERROR: la escritura en bytes excede al tamaño del sector\n";
#endif
  }

  file << data;
  file.close();
}

/*
INPUT: Id del sector
OUTPUT: retorna el objeto fstream asociado al sector, si existe
Permite abrir un sector con una cuenta vertical
Autor: Berly Dueñas
*/

fstream Disk::openNthSector(size_t sector_id) const{
  if (!doesSectorExist(sector_id)) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: Sector ID fuera de rango" << endl;
#endif
    return fstream();
  }
  if (sector_id == 0) {
#ifdef DEBUG
    // cerr<<"DISCO: WARNING se accedio al bloque 0\n";
#endif
  }
  pos sector_pos = getNthSector(sector_id);
  string filePath = diskRoot + "/platter_" + to_string(sector_pos.platter) +
                    "/surface_" + to_string(sector_pos.surface) +
                    "/track_" + to_string(sector_pos.track) +
                    "/sector_" + to_string(sector_pos.sector);
  fstream file(filePath, ios::in | ios::out);
  if (!file.is_open()) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: No se pudo abrir el sector " << sector_id
         << " en la ruta: " << filePath << endl;
#endif
    return fstream();
  }
  return file;
}

/*
 * INPUT: Id del sector
 * OUTPUT: string del path del sector
 * Permite abrir un sector con una cuenta vertical
 * Autor: Berly Dueñas
*/

string Disk::getSectorPath(size_t sector_id) const { 
  pos sector_pos = getNthSector(sector_id);
  string filePath = diskRoot + "/platter_" + to_string(sector_pos.platter) +
                    "/surface_" + to_string(sector_pos.surface) +
                    "/track_" + to_string(sector_pos.track) +
                    "/sector_" + to_string(sector_pos.sector);
  return filePath;
}

/*
INPUT: Id del sector
OUTPUT: objeto posicion fisica
Mapear un sector_id a posicion fisica del disco
Autor: Berly Dueñas
*/

pos Disk::getNthSector(size_t sector_id)const {
  if (sector_id >= totalSectors) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: Sector ID fuera de rango" << endl;
#endif
    return {0, 0, 0, 0};
  }
  return {
      sector_id /2 %(platters),               // platter
      sector_id % 2,                          // surface
      sector_id / (platters * 2 * sectors),   // track
      sector_id / (platters * 2) % sectors    // sector
  };
}

/*
INPUT: posicion del sector
OUTPUT: id del sector
Permite abrir un sector con una cuenta vertical
Autor: Berly Dueñas
*/

size_t Disk::getNthSector(pos sector_pos) const{
  if (!doesSectorExist(sector_pos)) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: Sector pos fuera de rango" << endl;
#endif
    return 0;
  }
  return sector_pos.platter * 2 * tracks * sectors +
         sector_pos.surface * tracks * sectors + sector_pos.track * sectors +
         sector_pos.sector;
}

size_t Disk::getSectorFreeSpace(size_t sector_id) const {
  fstream file = openNthSector(sector_id);
  if (!file.is_open()) {
    return 0;
  }
  file.seekg(0, ios::end);
  size_t fileSize = file.tellg();
  return sectorSize - fileSize;
}

/*
 * printDiskTree
 * Imprime el arbol de directorios del disco
 * Autor: Jafet Poco
 */
void Disk::printDiskTree() {
  for (size_t i = 1; i <= platters; i++) {
    cout << "Plato " << i << endl;
    cout << "├── Superficie 1" << endl;
    cout << "|   └── Pistas: " << tracks << endl;
    cout << "|       └── Sectores: " << sectors << endl;
    cout << "└── Superficie 2" << endl;
    cout << "    └── Pistas: " << tracks << endl;
    cout << "        └── Sectores: " << sectors << endl;
  }
  cout << "================================================\n";
}

/*
 * printDiskInfo
 * Imprime informacion general del disco
 * Autor: Jafet Poco
 */
void Disk::printDiskInfo() {
  printf("===================Disk info=======================\n");
  printf("Nombre del disco:\t\t%s\n", diskRoot.c_str());
  printf("Platos:\t\t\t\t%zd\n", platters);
  printf("Superficies por plato:\t\t%d\n", 2);
  printf("Pistas por superficie:\t\t%zd\n", tracks);
  printf("Sectores por pista:\t\t%zd\n", sectors);
  printf("Tamaño del sector:\t\t%zd bytes\n", sectorSize);
  printf("Capacidad del disco:\t\t%ld bytes (%.2F MB)\n", capacity,
         (double)capacity / (1024.f * 1024.f));
  printf("Sectores por bloque: \t\t%zd sectores.\n", blockLength);
  printf("------------------------------------------------\n");
}

/*
 * printSectorPos
 * metodo debug, para imprimir la posicion fisica del disco
 * Autor: Berly Dueñas
 */
void Disk::printSectorPos(size_t sectorId) {
  pos p = getNthSector(sectorId);
  cout<<p.platter << " "
      << p.surface << " "
      << p.track << " "
      << p.sector << endl; 
}

/*
 * printSectorCont
 * metodo debug, para imprimir el contenido de sector
 * Autor: Berly Dueñas
 */
void Disk::printSectorCont(size_t sector_id) {
  fstream file = openNthSector(sector_id);
  if (!file.is_open()) {
#ifdef DEBUG
    cerr << "DISCO: ERROR: No se pudo abrir el sector para imprimir" << endl;
#endif
    return;
  }
  string data = readSector(sector_id);
#ifdef DEBUG
  cerr << "Contenido del sector " << sector_id << ": \n";
  cerr<<data<<"\nTamaño sector "<<sector_id<<": "<<data.size()<<'\n';
#endif
}

/*
 * doesSectorExist
 * INPUT: posicion fisica en disco
 * OUTPUT: booleano
 * metodo debug, para saber si el sector existe dentro de una posicion fisica
 * Autor: Berly Dueñas
 */
bool Disk::doesSectorExist(pos sector_pos) const{
  if (sector_pos.platter >= platters || sector_pos.surface > 1 ||
      sector_pos.track >= tracks || sector_pos.sector >= sectors) {
    return false;
  }
  return true;
}

/*
 * doesSectorExist
 * INPUT: id de disco
 * OUTPUT: booleano
 * metodo debug, para saber si el sector existe dentro de un id de sector
 * Autor: Berly Dueñas
 */
bool Disk::doesSectorExist(size_t sector_id) const{
  pos sector_pos = getNthSector(sector_id);
  return doesSectorExist(sector_pos);
}

/*
 * doesSectorExist
 * INPUT: posicion fisica en disco
 * OUTPUT: booleano
 * metodo debug, para saber si el sector existe dentro de una posicion fisica
 * Autor: Jafet Poco
 */
bool Disk::isDiskOpen() {
  return fs::exists(diskRoot) && fs::is_directory(diskRoot);
}
