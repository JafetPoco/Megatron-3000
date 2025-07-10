#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include "hash.h"
#include "hash.cpp"

using namespace std;

struct Registro {
    int id;
    string contenido;
};

struct Chunk {
    int pageID;
    vector<Registro> registros;
};

// Limpia una línea: elimina comillas y reemplaza comas por #
string limpiarLinea(const string &linea) {
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
  return resultado;
}

// Procesa una línea CSV a un Registro
Registro procesarLinea(const string& linea) {
    Registro r;
    r.id = stoi(linea.substr(0, 4));
    r.contenido = limpiarLinea(linea) + '\n';
    return r;
}

// Construye chunks e inserta (id → pageID) en el hash extensible
void construirChunksConHash(
    istream& input,
    size_t chunkSize,
    vector<Chunk>& paginas,
    Directory& hashTable
) {
    string linea;
    getline(input, linea); // Saltar cabecera

    const size_t payloadSize = chunkSize - 4;
    Chunk actual{0};
    size_t tamanoActual = 0;

    while (getline(input, linea)) {
        if (linea.size() < 4) continue;

        Registro r = procesarLinea(linea);

        if (tamanoActual + r.contenido.size() > payloadSize) {
            // rellenar con '@' si sobra espacio
            if (tamanoActual < payloadSize) {
                Registro relleno{-1, string(payloadSize - tamanoActual, '@')};
                actual.registros.push_back(relleno);
            }
            paginas.push_back(actual);
            actual = Chunk{actual.pageID + 1};
            tamanoActual = 0;
        }

        // Insertar en hash: id → pageID como string
        string pageIDstr = to_string(actual.pageID);
        hashTable.insert(r.id, pageIDstr, false);

        actual.registros.push_back(r);
        tamanoActual += r.contenido.size();
    }

    // Último chunk
    if (!actual.registros.empty()) {
        if (tamanoActual < payloadSize) {
            Registro relleno{-1, string(payloadSize - tamanoActual, '@')};
            actual.registros.push_back(relleno);
        }
        paginas.push_back(actual);
    }
}

// Consulta tipo SQL: SELECT * WHERE id = ?
void selectPorID(int id, Directory& hashTable) {
    cout << "Consulta SELECT * WHERE id = " << id << endl;
    hashTable.search(id);
}

// Imprime todos los chunks
void mostrarChunks(const vector<Chunk>& paginas) {
    for (const auto& chunk : paginas) {
        cout << "==== Chunk " << setw(4) << setfill('0') << chunk.pageID << " ====" << endl;
        for (const auto& reg : chunk.registros) {
            if (reg.id != -1)
                cout << reg.contenido;
        }
        cout << endl;
    }
}

int main() {
    const string nombreArchivo = "datos.csv";
    const size_t chunkSize = 64; // bytes por chunk (incluye 4 bytes de header)
    const int profundidadInicial = 2;
    const int tamanoBucket = 2;

    Directory hashTable(profundidadInicial, tamanoBucket);
    vector<Chunk> paginas;

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir " << nombreArchivo << endl;
        return 1;
    }

    construirChunksConHash(archivo, chunkSize, paginas, hashTable);
    archivo.close();

    // Mostrar chunks
    mostrarChunks(paginas);

    // Búsqueda simulada tipo SQL
    int consultaID;
    cout << "\nIngrese un ID para buscar: ";
    cin >> consultaID;
    selectPorID(consultaID, hashTable);

    return 0;
}
