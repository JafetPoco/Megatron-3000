#define DEBUG
#define VERBOSE
#include "tableFiles.h"
#include "freeBlockMan.h"
#include "block.h"
#include "globals.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

static constexpr size_t METADATA_LENGTH = 40;
static constexpr size_t ENTRY_SIZE = 34;
static constexpr size_t NAME_SIZE = 30;
static constexpr size_t BLOCK_SIZE = 4;
static string BLOCK_HEAD_PTR="0000";

/*
Lee el directorio del disco, y crea un mapa con el nombre
de un archivo y el ID del bloque donde empieza
Autor: Jafet Poco
*/
TableFiles::TableFiles() { 
  #ifdef VERBOSE
  std::cerr<<"TF: Leyendo tabla de archivos...\n";
  #endif
  loadTable();
  #ifdef VERBOSE
  std::cerr<<"TF: Tabla de archivos se cargo correctamente.\n";
  #endif
}

void TableFiles::loadTable() {
  table.clear();

  auto info = disk->info();
  size_t sectorSize = info.sectorSize;

  std::string sector = disk->readSector(0);
#ifdef DEBUG
  std::cerr << "TF: Sector 0:\n" << sector << '\n';
#endif
  if (sector.size() < sectorSize) {
    sector.append(sectorSize - sector.size(), '_');
  }

  size_t offset = METADATA_LENGTH;
  while (offset + ENTRY_SIZE <= sector.size()) {

    std::string rawName = sector.substr(offset, NAME_SIZE);
    size_t endPos = rawName.find('_');
    std::string name =
        (endPos == std::string::npos ? rawName : rawName.substr(0, endPos));

    std::string numStr = sector.substr(offset + NAME_SIZE, BLOCK_SIZE);
    int block = 0;
    try {
      block = std::stoi(numStr);
    } catch (...) {
      break; 
    }

    if (!name.empty()) {
#ifdef DEBUG
      std::cerr << "TF: Archivo encontrado: " << name << ", bloque: " << block << std::endl;
#endif
      table[name] = static_cast<size_t>(block);
    }
    offset += ENTRY_SIZE;
  }
}

/*
INPUT: Nombre del archivo
OUTPUT: Verdadero si el archivo existe o Falso de lo contrario
Busca un archivo por el nombre dentro del mapa (directorio)
Autor: Jafet Poco
*/

BlockID TableFiles::findFile(std::string name) {
  #ifdef VERBOSE
  std::cerr<<"TF: buscando archivo: "<<name<<'\n';
  #endif
  auto it = table.find(name);
  if (it == table.end()){
#ifdef VERBOSE
    std::cerr<<"TF: no se encontro el archivo: "<<name<<'\n';
#endif
    return -1;
  }
#ifdef DEBUG
  std::cerr << "TF: Archivo encontrado, bloque: " << it->second << std::endl;
#endif
  return it->second;
}

/*
Imprime la tabla de archivos
Autor: Jafet Poco
*/
void TableFiles::showTable(){
  if(table.size() == 0){
#ifdef VERBOSE
    std::cerr<<"TF: WARNNING: No hay archivos en el disco\n";
#endif
    return;
  }
  std::cerr<<"================================\n";
  std::cerr<<"Tabla de Archivos: \n";
  std::cerr<<"--------------------------------\n";
  std::cerr<<"Nombre\t\tBloque\n";
  for(const auto &par: table){
    std::cerr<<par.first<<"\t\t"<<par.second<<"\n";
  }
  std::cerr<<"================================\n";
}

/*
INPUT: Disco
Escribe los cambios realizados en el disco
Autor: Jafet Poco
*/

void TableFiles::saveTable() {
  auto info = disk->info();
  size_t sectorSize = info.sectorSize;

  std::string sector = disk->readSector(0);
#ifdef DEBUG
  std::cerr<<"TF: Obtenido del disco: \n"<<sector<<std::endl;
#endif
  if (sector.empty()) {
    sector.assign(sectorSize, '_');
  }

  sector.resize(METADATA_LENGTH);
  std::string metadata;
  for (const auto &entry : table) {
#ifdef DEBUG
    std::cerr<<"TF: Guardando entrada: "<<entry.first<<", bloque: "<<entry.second<<std::endl;
#endif
    std::string name = entry.first;
    int value = entry.second;

    if (name.length() > 30)
      name = name.substr(0, 30);
    else
      name.append(30 - name.length(), '_');

    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%04d", value);

    metadata += name + buffer;
  }

  sector.replace(METADATA_LENGTH, metadata.size(), metadata);

  ofstream sectorF(disk->getSectorPath(0), ios::trunc);
  sectorF<<sector;
}
/*
INPUT: Nombre del archivo y el ID del bloque donde empieza
Agrega un archivo al mapa (directorio)
Autor: Jafet Poco
*/

BlockID TableFiles::addFile(std::string name) {
  if (table.find(name) != table.end()) {
#ifdef VERBOSE
    std::cerr << "TF: El archivo '" << name << "' ya existe.\n";
#endif
    return table.find(name)->second;
  }
  BlockID block = freeBlock->allocateBlock();
  if (block == -1) {
#ifdef VERBOSE
    std::cerr<<"TF: No se pueden almacenar mas archivos...\n";
#endif
    return -1;
  }
  table[name] = block;
  saveTable();
  // Frame *f = buffer->accessPage(block, 'w');
  // auto& ss=f->data->getData();
  // ss=BLOCK_HEAD_PTR; //"0000"
  // f->data->saveBlock();
#ifdef VERBOSE
  std::cerr << "TF: Archivo '" << name << "' agregado en bloque " << block << ".\n";
#endif
  return block;
}
