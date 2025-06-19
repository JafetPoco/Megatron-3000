#include "file.h"
#include "disk.h"
#include "bufPool.h"
#include "freeBlockMan.h"
#include "tableFiles.h"

#include "globals.h"
Disk* disk=nullptr;
BufPool* bufferPool=nullptr;

FreeBlockManager *freeBlock = nullptr;

TableFiles *tableFile = nullptr;
int main() {
  // Inicializamos disco (ya creado antes, no se vuelve a formatear)
  
  // disk = new Disk("Megatron", 1, 1, 128, 512, 4);
  disk = new Disk("Megatron");
  disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", 40);
  tableFile = new TableFiles(disk);
  tableFile->showTable();
  bufferPool = new BufPool(3);
  // 3 frames en buffer pool

  // Creamos un archivo lógico
  File f("prueba");

  // // Escribimos registros de prueba más grandes que un bloque
  std::string contenido;
  contenido = std::string(20, 'A');  // llenar el resto del bloque

  f.write(contenido);

  contenido = string(2044, 'a');
  f.write(contenido);
  std::cout<<disk->getSectorPath(1*4);
}
