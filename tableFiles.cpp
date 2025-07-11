#include "tableFiles.h"
#include "block.h"
#include <iostream>
#include <sstream>
#include <iomanip>

/*
INPUT: el Disco
Lee el directorio del disco, y crea un mapa con el nombre
de un archivo y el ID del bloque donde empieza
Autor: Jafet Poco
*/

TableFiles::TableFiles(Disk *&disk) {
  std::string buffer = disk->readSector(0);

  if(buffer.size() <= 40) return;
  for(size_t offset = 40; buffer[offset] != '\0'; offset += 34){
    std::string rawName = buffer.substr(offset, 30);
    size_t endName = rawName.find('_');
    std::string name = (endName != std::string::npos)
                         ? rawName.substr(0, endName)
                         : rawName;

    size_t numBlock = stoi(buffer.substr(offset+30, 4));
    
    table.insert({name, numBlock});
  }
}

/*
INPUT: Nombre del archivo
OUTPUT: Verdadero si el archivo existe o Falso de lo contrario
Busca un archivo por el nombre dentro del mapa (directorio)
Autor: Jafet Poco
*/

bool TableFiles::findFile(std::string name, size_t *position){
  auto iterador = table.find(name);
  if(iterador != table.end()) {
    *position = iterador->second;
    return true;
  } else {
    return false;
  }
}

void TableFiles::showTable(){
  if(table.size() == 0){
    std::cerr<<"WARNNING: No hay archivos en el disco\n";
    return;
  }
  std::cout<<"================================\n";
  std::cout<<"Tabla de Archivos: \n";
  std::cout<<"--------------------------------\n";
  std::cout<<"Nombre\t\tBloque\n";
  for(const auto &par: table){
    printf("%s\t\t%ld\n", par.first.c_str(), par.second);
  }
  std::cout<<"================================\n";
}

/*
INPUT: Disco
Escribe los cambios realizados en el disco
Autor: Jafet Poco
*/

void TableFiles::saveChanges(){
  std::stringstream directory;
  for(auto file: table){
    directory << std::left << std::setfill('_') << std::setw(30) << file.first;
    directory << std::right << std::setfill('0') << std::setw(4) << file.second;
  }
  std::string directoryStr = directory.str();

  std::string dataBlock = disk->readSector(0);
  dataBlock.replace(40, directoryStr.length(), directoryStr);
  disk->writeSector(0, dataBlock);
}

/*
INPUT: Nombre del archivo y el ID del bloque donde empieza
Agrega un archivo al mapa (directorio)
Autor: Jafet Poco
*/

void TableFiles::addFile(std::string nameFile, size_t id){
  table.insert({nameFile, id});
}