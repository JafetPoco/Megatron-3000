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

// Guarda chunks en memoria y simula impresión de escritura
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
      cout << "GUARDADO >> Chunk " << setw(4) << setfill('0') << actual.pageID << ":\n";
      for (const auto& reg : actual.registros)
        cout << reg.contenido;
      cout << "\n----------------------------\n";

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

    cout << "GUARDADO >> Chunk " << setw(4) << setfill('0') << actual.pageID << ":\n";
    for (const auto& reg : actual.registros)
      cout << reg.contenido;
    cout << "\n----------------------------\n";

    paginas.push_back(actual);
  }

  return paginas;
}

// SELECT * WHERE id = ?
void selectWhere(int id, Directory &hashTable, const vector<Chunk> &paginas) {
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
      cout << "Encontrado en chunk " << setw(4) << setfill('0') << pageID << ":\n";
      cout << reg.contenido;
      return;
    }
  }

  cout << "ID no encontrado dentro del chunk esperado." << endl;
}

void selectAll(vector<Chunk> &paginas) {
  cout << "SELECT * "<< endl;
  for (const auto &chunk : paginas) {
    // cout << "==== Chunk " << setw(4) << setfill('0') << chunk.pageID << " ====" << endl;
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

  ifstream archivo(nombreArchivo);
  if (!archivo.is_open()) {
    cerr << "No se pudo abrir " << nombreArchivo << endl;
    return 1;
  }

  vector<Chunk> paginas = chunkSplit(archivo, chunkSize, hashTable);
  archivo.close();

  hashTable.display(true);

  int opcion;
  cout << "\n1. SELECT * from titanic\n2. SELECT * WHERE id = ?\nOpción: ";
  cin >> opcion;

  if (opcion == 1) {
    selectAll(paginas);
  } else if (opcion == 2) {
    int id;
    cout << "Ingrese ID: ";
    cin >> id;
    selectWhere(id, hashTable, paginas);
  }

  return 0;
}
