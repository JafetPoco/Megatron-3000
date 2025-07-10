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

void buildIndex(istream &input, size_t chunkSize, vector<Chunk> &v,
                Directory &h) {
  string a;
  getline(input, a);

  const size_t p = chunkSize - 4;
  Chunk actual{0};
  size_t tam = 0;

  while (getline(input, a)) {
    if (a.size() < 4) continue;

    Registro b = lineProcess(a);

    if (tam + b.contenido.size() > p) {
      if (tam < p) actual.registros.push_back({-1, string(p - tam, '@')});
      v.push_back(actual);
      actual = Chunk{actual.pageID + 1};
      tam = 0;
    }

    h.insert(b.id, to_string(actual.pageID), false);
    actual.registros.push_back(b);
    tam += b.contenido.size();
  }

  // Último chunk
  if (!actual.registros.empty()) {
    if (tam< p) {
      Registro relleno{-1, string(p- tam, '@')};
      actual.registros.push_back(relleno);
    }
    v.push_back(actual);
  }
}

// Consulta tipo SQL: SELECT * WHERE id = ?
void selectWhere(int id, Directory &hashTable) {
  cout << "SELECT * WHERE id = ";
  cin >> id;
  hashTable.search(id);
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

  buildIndex(archivo, chunkSize, paginas, hashTable);
  archivo.close();

  hashTable.display(1);

  mostrarChunks(paginas);

  int consultaID;
  selectWhere(consultaID, hashTable);

  return 0;
}
