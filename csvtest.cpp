#include <fstream>
#include "disk.h"
#include "block.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include "hash.h"
#include "hash.cpp"

using namespace std;

string clean_line(const string &linea) {
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

struct Registro {
  int id;
  string contenido;
};

struct Chunk {
  int pageID;
  vector<Registro> registros;
};


vector<Chunk> chunkSplit(istream &input, size_t chunkSize, Directory *hashTable = nullptr) {
  const size_t headerSize = 4;
  const size_t payloadSize = chunkSize - headerSize;

  string linea;
  getline(input, linea);  // Saltar cabecera

  vector<Chunk> paginas;
  Chunk actual{0};
  size_t tam = 0;

  while (getline(input, linea)) {
    if (linea.size() < 4) continue;

    Registro r;
    r.id = stoi(linea.substr(0, 4));
    r.contenido = clean_line(linea) + '\n';

    if (tam + r.contenido.size() > payloadSize) {
      if (tam < payloadSize) {
        actual.registros.push_back({-1, string(payloadSize - tam, '@')});
      }
      paginas.push_back(actual);
      actual = Chunk{actual.pageID + 1};
      tam = 0;
    }

    if (hashTable) {
      hashTable->insert(r.id, to_string(actual.pageID), false);
    }

    actual.registros.push_back(r);
    tam += r.contenido.size();
  }

  if (!actual.registros.empty()) {
    if (tam < payloadSize) {
      actual.registros.push_back({-1, string(payloadSize - tam, '@')});
    }
    paginas.push_back(actual);
  }

  return paginas;
}
int main() {
  ifstream archivo("titanic.csv");
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    return 1;
  }

  size_t chunkSize = 512*4;
  vector<Chunk> chunks = chunkSplit(archivo, chunkSize);

  cout << "Chunks generados: " << chunks.size() << endl;
  if (!chunks.empty()) {
    for (auto& i: chunks) {
      cout<<i.pageID<<"||||"<<i.registros.size()<<"\n\n";
    }
  }
  return 0;
}
