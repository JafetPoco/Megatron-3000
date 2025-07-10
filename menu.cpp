#include "menu.h"
#include "disk.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include "tableFiles.h"
#include "freeBlockMan.h"
#include "file.h"
#include "bufPool.h"
#include "recordManager.h"
#include "globals.h"

#define DEBUG si

Disk *disk = nullptr;
TableFiles *tableFile = nullptr;
FreeBlockManager *freeBlock = nullptr;
BufPool*bufferPool = nullptr;
size_t sectorPerBlock;
size_t numBlock;

void AuxMenu() {
  printf("----------------------------------------\n");
  printf("Bienvenido al sistema Megatron 3000\n");
  printf("Selecciona una opcion: \n");
  printf("1. Formatear Disco\n");
  printf("2. Visualizar Buffer\n");
  printf("3. Ver Sector\n");
  printf("4. Ver Bloque\n");
  printf("5. Subir CSV\n");
  printf("6. Select\n");
  printf("0. Exit\n");
  printf("----------------------------------------\n");
  printf("Ingrese su opcion: ");
  ssize_t option;
  scanf("%zu", &option);
  switch (option) {
  case 1:
    delete disk;
    disk = new Disk("Megatron", 8, 4, 4, 512, 4);
    printf("Disco Formateado...\n");
    break;
  case 2:{
    int op, nFrame;
    printf("1) LRU\n");
    printf("2) Clock\n");
    printf("-> "); std::cin>>op;
    printf("Numero de frames: "); std::cin>>nFrame;
    switch (op){
    case 1:
      delete bufferPool;
      bufferPool = new LRU(nFrame);
      menu_buffer();
      break;
    case 2:
      delete bufferPool;
      bufferPool = new Clock(nFrame);
      menu_buffer();
      break;
    default:
      printf("Opción inválida, regresando a menu...\n");
      break;
    }
    break;
  }
  case 3:{
    size_t sector_id;
    printf("Ingrese el ID del sector a leer: ");
    scanf("%zu", &sector_id);
    std::string data = disk->readSector(sector_id);
    std::cout<<"Contenido del sector "<<sector_id<<": "<<data<<"\n";
    break;
  }
  case 4:{
    size_t idBlock;
    printf("Ingrese ID del bloque: "); std::cin>>idBlock;
    Block b(idBlock);
    std::cout<<"Data del bloque "<<idBlock<<"\n"<<b.getData()<<"\n";
    break;
  }
  case 5:{
    RecordManagerFixed rm;
    std::string name;
    printf("Nombre del .CSV: \n"); std::cin>>name;
    rm.readCSV(name);
    bufferPool->clearBuffer();
    break;
  }
  case 6:{
    RecordManagerFixed rm;
    std::string name;
    printf("Nombre de la tabla: \n"); std::cin>>name;
    rm.select(name);
    break;
  }
  case 0:
    printf("Saliendo del sistema Megatron 3000...\n");
    exit(0);
  default:
    printf("Opcion invalida. Intente de nuevo.\n");
    break;
  }
}

void menu_buffer(){
  while(true){
    printf("1. Request Page\n");
    printf("2. Pin Frame\n");
    printf("3. Unpin Frame\n");
    printf("0. Exit\n");
    printf("----------------------------------------\n");
    printf("Ingrese su opcion: ");
    ssize_t option;
    scanf("%zu", &option);
    switch (option) {
    case 1:{
      size_t idBlock;
      char mode;
      printf("Id bloque: "); std::cin>>idBlock;
      printf("r/w: "); std::cin>>mode;
      std::string *data = &bufferPool->requestPage(idBlock, mode);
      std::string frase;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      if(mode == 'w'){
        std::cout<<"Ingrese texto: "; getline(std::cin, frase);
        *data = frase;
      } else if(mode == 'r'){
        std::cout<<"Información del bloque: \n"<<*data<<"\n";
      }
      bufferPool->print(idBlock, 'g');
      break;
    }
    case 2:
      size_t block;
      printf("ID del bloque: "); std::cin>>block;
      bufferPool->pinFrame(block);
      bufferPool->print(block, 'y');
      break;
    case 3:
      size_t Idblock;
      printf("ID del bloque: "); std::cin>>Idblock;
      bufferPool->unPinFrame(Idblock);
      bufferPool->print(Idblock, 'y');
      break;
    case 0:
      printf("Saliendo del buffer...\n");
      bufferPool->clearBuffer();
      bufferPool->printEstadistic();
      return;
    default:
      printf("Opcion invalida. Intente de nuevo.\n");
      break;
    }
  }
}

void menu() {
  disk = new Disk("Megatron");
  freeBlock = new FreeBlockManager("Megatron", 100);
  tableFile = new TableFiles(disk);
  bufferPool = new LRU(4);
  while (true) {
    AuxMenu();
  }
}
