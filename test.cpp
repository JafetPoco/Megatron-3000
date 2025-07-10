#include "hash.h"
#include "hash.cpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

struct Registro {
  int id;
  string contenido;
};

struct Chunk {
  int pageID;
  vector<Registro> registros;
};

string clean_line(string &linea) {
  string resultado;
  bool dentroDeComillas = false;
  for (char c : linea) {
    if (c == '"') {
      dentroDeComillas = !dentroDeComillas;
    } else if (c == ',' && !dentroDeComillas) {
      resultado += '#';
    } else {
      resultado += c;
    }
  }
  resultado += "%";
  return resultado;
}

Registro lineProcess(string &linea) {
  Registro r;
  r.id = stoi(linea.substr(0, 4));
  r.contenido = clean_line(linea) + '\n';
  return r;
}

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

void selectWhere(int id, Directory &hashTable, const vector<Chunk>& paginas) {
  cout << "SELECT * WHERE id = " << id << endl;

  string pageIDstr;
  if (!hashTable.search(id)) {
    cout << "ID no encontrado en el índice." << endl;
    return;
  }

  int pageID = stoi(pageIDstr);
  if (pageID < 0 || pageID >= (int)paginas.size()) {
    cout << "pageID fuera de rango." << endl;
    return;
  }

  const Chunk& chunk = paginas[pageID];
  for (const auto& reg : chunk.registros) {
    if (reg.id == id) {
      cout << "Encontrado en chunk " << setw(4) << setfill('0') << pageID << ":\n";
      cout << reg.contenido;
      return;
    }
  }

  cout << "ID no encontrado dentro del chunk esperado." << endl;
}


void selectAll(const vector<Chunk> &paginas) {
  cout << "SELECT * (lectura secuencial de todos los registros)" << endl;
  for (const auto &chunk : paginas) {
    cout << "==== Chunk " << setw(4) << setfill('0') << chunk.pageID
         << " ====" << endl;
    for (const auto &reg : chunk.registros) {
      if (reg.id != -1) // Ignorar relleno
        cout << reg.contenido;
    }
    cout << endl;
  }
}

// Imprime todos los chunks
void mostrarChunks(const vector<Chunk> &paginas) {
  for (const auto &chunk : paginas) {
    cout << "==== Chunk " << setw(4) << setfill('0') << chunk.pageID
         << " ====" << endl;
    for (const auto &reg : chunk.registros) {
      if (reg.id != -1)
        cout << reg.contenido;
    }
    cout << endl;
  }
}

int main() {
  const string nombreArchivo = "titanic.csv";
  const size_t chunkSize = 512;
  const int profundidadInicial = 1;
  const int tamanoBucket = 2;

  Directory hashTable(profundidadInicial, tamanoBucket);
  vector<Chunk> paginas;

  ifstream archivo(nombreArchivo);
  if (!archivo.is_open()) {
    cerr << "No se pudo abrir " << nombreArchivo << endl;
    return 1;
  }

  chunkSplit(archivo, chunkSize, paginas, hashTable);
  archivo.close();

  hashTable.display(1);

  mostrarChunks(paginas);

  int consultaID;
  selectWhere(consultaID, hashTable);

  return 0;
}
