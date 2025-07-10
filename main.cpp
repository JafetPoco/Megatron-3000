#include "bufPool.h"
#include "file.h"
#include "disk.h"
#include "block.h"
#include "freeBlockMan.h"
#include "tableFiles.h"
#include "globals.h"
#include <iostream>
using namespace std;

Disk* disk = nullptr;
FreeBlockManager* freeBlock=nullptr;
TableFiles* tableFile=nullptr;
BufPool* bufferPool = nullptr;
size_t blockCapacity;

void testBuffer();
string clean_line(string &linea);
vector<string> chunkSplit(istream &input, size_t chunkSize);

int main() {
  disk = new Disk("Megatron", 2,2,8,512,4);
  // disk = new Disk("Megatron");
  blockCapacity = disk->info().sectorSize * disk->info().blockLength;
  cerr<<disk->getTotalSectors()<<endl;
  disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", 256);
  tableFile = new TableFiles(); //carga si existe el disco
  bufferPool = new Clock(5);
  
  ifstream archivo("titanic.csv");
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    return 1;
  }

  cout << "Capacidad: "<<blockCapacity<<endl;
  vector<string> chunks = chunkSplit(archivo, blockCapacity);

  cout << "Chunks generados: " << chunks.size() << endl;
  if (!chunks.empty()) {
    for (auto& i: chunks) {
      cout<<i<<"\n\n";
    }
  }
  
  for (int i=0;i<chunks.size();i++) {
    Block page(i+1);
    string& data = page.getData();
    cout<<"**********************Capacidad de chunk: "<<chunks[i].size()<<endl; 
    data = chunks[i];
    page.saveBlock();
  }
 
  // tableFile->addFile("test1");
  // tableFile->addFile("test2");
  // tableFile->addFile("test3");
  // tableFile->addFile("test4");
  // tableFile->addFile("test5");
  // tableFile->addFile("test6");
  // tableFile->addFile("test7");
  // tableFile->addFile("test8");
  // tableFile->addFile("test10");
  // tableFile->addFile("test11");
  // tableFile->addFile("test12");
  // tableFile->addFile("test13");
  // tableFile->addFile("test14");
  // tableFile->addFile("test15");
  // tableFile->addFile("test16");
  // tableFile->addFile("test17");
  // tableFile->addFile("test18");
  // tableFile->addFile("test19");
  // tableFile->addFile("test20");
  // tableFile->addFile("test21");
  // tableFile->addFile("test22");
  // tableFile->addFile("test23");
  // tableFile->addFile("test24");
  // tableFile->addFile("test25");
  // tableFile->addFile("test26");
  // tableFile->showTable();

//   page.openBlock(0);
//   cout<<page.getData()<<endl;
  // testBuffer();
}

/****************************************************/
/****************************************************/
/****************************************************/

bool createDisk() {
  disk =new Disk("Megatron", 1,2,4,512,8);
  disk->printDiskInfo();
  // for (int i = 0; i<disk.getTotalSectors(); i++) {
  //   cout<<disk.getSectorPath(i)<<'\n';
  // }
  cout<<"Total sectors: "<<disk->getTotalSectors()<<'\n';
  return true;
}
bool testDisk() {

  createDisk();
  if (!disk->isDiskOpen()) {
    return false;
  }
  cerr<<"ROOT DISK: "<<disk->getDiskRoot()<<'\n';
  disk->printDiskInfo();
  disk->printDiskTree();
  cerr<<"Contenido sector 0\n";
  disk->printSectorCont(0);
  return true;
}

bool testTableFiles() {
  tableFile = new TableFiles();
  return true;
}

void testBuffer() {
  bufferPool = new LRU(4);
  bufferPool->requestPage(0, 'r');
  bufferPool->requestPage(1, 'r');
  bufferPool->requestPage(2, 'r');
  bufferPool->requestPage(3, 'r');
  string& test = bufferPool->requestPage(0, 'r');
  cout<<"Data in block 0: "<<test<<endl;
}

string clean_line(string &linea) {
  string resultado;
  bool quote = false;
  for (char c : linea) {
    if (c == '"') {
      quote = !quote;
    } else if (c == ',' && !quote) {
      resultado += '#';
    } else {
      resultado += c;
    }
  }
  resultado+="%";
  return resultado;
}

vector<string> chunkSplit(istream &input, size_t chunkSize) {
  const size_t headerSize = 4;
  if (chunkSize <= headerSize) {
    cerr << "ERROR: chunkSize debe ser mayor que 4." << endl;
    return {};
  }

  string linea;
  getline(input, linea);

  vector<string> registros;
  while (getline(input, linea)) {
    registros.push_back(clean_line(linea));
  }

  size_t payloadSize = chunkSize - headerSize;
  size_t contador = 1;
  string buffer;
  vector<string> chunks;

  for (const auto &registro : registros) {
    if (buffer.size() + registro.size() > payloadSize) {
      string header = to_string(contador++);
      header = string(4 - header.size(), '0') + header;

      buffer.resize(payloadSize, '@');
      chunks.push_back(header + buffer);

      buffer.clear();
    }
    buffer += registro;
  }

  if (!buffer.empty()) {
    string header = to_string(contador++);
    header = string(4 - header.size(), '0') + header;

    buffer.resize(payloadSize, '@');
    chunks.push_back(header + buffer);
  }

  return chunks;
}
