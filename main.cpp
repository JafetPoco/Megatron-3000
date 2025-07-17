#include "bufPool.h"
#include "hash.h"
#include "file.h"
#include "disk.h"
#include "block.h"
#include "freeBlockMan.h"
#include "schema.h"
#include "tableFiles.h"
#include "globals.h"
#include <iostream>
using namespace std;

Disk* disk = nullptr;
FreeBlockManager* freeBlock=nullptr;
TableFiles* tableFile=nullptr;
BufPool* bufferPool = nullptr;
size_t blockCapacity;

struct Registro {
  int id;
  string contenido;
};

struct Chunk {
  int pageID;
  vector<Registro> registros;
};

void testBuffer();
string clean_line(string &linea);
vector<Chunk> chunkSplit(istream &input, size_t chunkSize, Directory &hashTable);
void selectWhere(int id, Directory &hashTable, vector<Chunk> &paginas);
void selectAll(vector<Chunk> &paginas);
void saveBlocks(vector<Chunk>& paginas);
void menuQ(vector<Chunk>& paginas, Directory& hashTable);

int main() {
  disk = new Disk("Megatron", 8,8,8,512,4); 
  //disk = new Disk("Megatron");
  blockCapacity = disk->info().sectorSize * disk->info().blockLength;
  cerr<<disk->getTotalSectors()<<endl;
  disk->printDiskInfo();
  freeBlock = new FreeBlockManager("Megatron", 256);
  tableFile = new TableFiles(); //carga si existe el disco
  bufferPool = new Clock(5);

  File::set_capacity(blockCapacity);

  File schemafile("schema", 'w');
  cout<<"CAPACITY: "<<schemafile.getCapacity()<<endl;
  string& schemaData = schemafile.accessBlock();
  schemaData = "titanic#id#4#LONG#name#50#STRING#age#3#LONG#fare#8#DOUBLE\n";
  cout<<"Next block???: "<<schemafile.getNext()<<endl;
  cout<<"Schema data: "<<schemaData<<endl;
  schemafile.close();

  bufferPool->clearBuffer();

  Block test(1);
  cout<<"Block ID: "<<test.getData()<<endl;

  // bufferPool->clearBuffer();



  // Schema schema("titanic");
  // schema.printSchema();
  // ifstream archivo("titanicG.csv");
  // if (!archivo) {
  //   cerr << "No se pudo abrir el archivo" << endl;
  //   return 1;
  // }

  // Directory hashTable(1, 8);

  // cout << "Capacidad: "<<blockCapacity<<endl;
  // vector<Chunk> paginas = chunkSplit(archivo, blockCapacity, hashTable);
  // archivo.close();

  // saveBlocks(paginas);

  // hashTable.display(0);

  // menuQ(paginas, hashTable);


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

Registro lineProcess(string &linea) {
  Registro r;
  r.id = stoi(linea.substr(0, 4));
  r.contenido = clean_line(linea) + '\n';
  return r;
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

vector<Chunk> chunkSplit(istream &input, size_t chunkSize, Directory &hashTable) {
  const size_t headerSize = 4;
  const size_t payloadSize = chunkSize - headerSize;

  string linea;
  getline(input, linea); // saltar cabecera

  vector<Chunk> paginas;
  Chunk actual{0};
  size_t tam = 0;

  while (getline(input, linea)) {
    if (linea.size() < 4) continue;

    Registro r = lineProcess(linea);

    if (tam + r.contenido.size() > payloadSize) {
      if (tam < payloadSize) {
        actual.registros.push_back({-1, string(payloadSize - tam, '@')});
      }

      // Simular guardado con cout
      cout << "GUARDADO >> pagina " << setw(4) << setfill('0') << actual.pageID << ":\n";
      // for (const auto& reg : actual.registros)
        // cout << reg.contenido;
      // cout << "\n----------------------------\n";

      paginas.push_back(actual);
      actual = Chunk{actual.pageID + 1};
      tam = 0;
    }

    hashTable.insert(r.id, to_string(actual.pageID), false);
    actual.registros.push_back(r);
    tam += r.contenido.size();
  }

  if (!actual.registros.empty()) {
    if (tam < payloadSize) {
      actual.registros.push_back({-1, string(payloadSize - tam, '@')});
    }

    cout << "GUARDADO >> pagina " << setw(4) << setfill('0') << actual.pageID << ":\n";
    // for (const auto& reg : actual.registros)
    //   cout << reg.contenido;
    // cout << "\n----------------------------\n";

    paginas.push_back(actual);
  }

  return paginas;
}

void selectWhere(int id, Directory &hashTable, vector<Chunk> &paginas) {
  cout << "SELECT * WHERE id = " << id << endl;

  string pageIDstr = hashTable.search(id);
  if (pageIDstr == "") {
    cout << "ID no encontrado en el índice." << endl;
    return;
  }

  int pageID = stoi(pageIDstr);
  if (pageID < 0 || pageID >= (int)paginas.size()) {
    cout << "pageID fuera de rango." << endl;
    return;
  }

  const Chunk &chunk = paginas[pageID];
  for (const auto &reg : chunk.registros) {
    if (reg.id == id) {
      cout << "Encontrado en pagina " << setw(4) << setfill('0') << pageID << ":\n";
      cout << reg.contenido;
      return;
    }
  }

  cout << "ID no encontrado dentro de la pagina esperada." << endl;
}

void selectAll(vector<Chunk> &paginas) {
  cout << "SELECT * "<< endl;
  for (const auto &chunk : paginas) {
    // cout << "==== Chunk " << setw(4) << setfill('0') << chunk.pageID << " ====" << endl;
    for (const auto &reg : chunk.registros) {
      if (reg.id != -1)
        cout << reg.contenido;
    }
    // cout << endl;
  }
  cout<<endl;
}

void saveBlocks(vector<Chunk>& paginas) {
  const size_t headerSize = 4;
  const size_t payloadSize = blockCapacity- headerSize;

  for (int i = 0; i < paginas.size(); ++i) {
    const Chunk& chunk = paginas[i];
    string text;

    for (const auto& reg : chunk.registros) {
      text += reg.contenido;
    }

    if (text.size() < payloadSize) {
      text.resize(payloadSize, '@');
    } else if (text.size() > payloadSize) {
      cerr << "ERROR: La pagina " << i << " excede el tamaño permitido\n";
      continue;
    }

    string header = to_string(i + 1);
    header = string(headerSize - header.size(), '0') + header;

    string& pagina = bufferPool->requestPage(i + 1, 'w');
    pagina = header + text;
    // cout<<"SAVE: "<<pagina.size()<<"     "<<header<<'\n'<<pagina;
  }
}

void menuQ(vector<Chunk>& paginas, Directory& hashTable) {
  while (true) {
    cout << "\n==================== MENÚ ====================\n";
    cout << "1. Mostrar información del disco\n";
    cout << "2. SELECT * FROM titanic\n";
    cout << "3. SELECT * WHERE id = ?\n";
    cout << "4. print índice hash\n";
    cout << "0. Salir\n";
    cout << "=============================================\n";
    cout << "Seleccione una opción: ";

    int opcion;
    cin >> opcion;

    switch (opcion) {
      case 1:
        disk->printDiskInfo();
        break;

      case 2:
        selectAll(paginas);
        break;

      case 3: {
        int id;
        cout << "Ingrese ID a buscar: ";
        cin >> id;
        selectWhere(id, hashTable, paginas);
        break;
      }

      case 4:
        hashTable.display(1);
        break;

      case 0:
        cout << "Saliendo...\n";
        return;

      default:
        cout << "Opción inválida. Intente de nuevo.\n";
        break;
    }
  }
}
