#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
string limpiarLinea(string &linea) {
  string resultado;
  for (char c : linea) {
    if (c == ',')
      resultado += '#';
    else if (c != '"')
      resultado += c;
  }
  return resultado;
}
string leerCSVCompleto(string &nombreArchivo) {
  ifstream archivo(nombreArchivo, ios::in);
  if (!archivo.is_open()) {
    cerr << "ERROR: No se pudo abrir el archivo: " << nombreArchivo << endl;
    exit(1);
  }

  stringstream buffer;
  buffer << archivo.rdbuf(); // Cargar todo el archivo en memoria
  return buffer.str();
}

void dividirEnChunks(istream &input, size_t chunkSize) {
  const size_t headerSize = 4;
  if (chunkSize <= headerSize) {
    cerr << "ERROR: chunkSize debe ser mayor que 4." << endl;
    return;
  }

  string linea;
  getline(input, linea); // Ignora la cabecera

  vector<string> registros;
  while (getline(input, linea)) {
    registros.push_back(limpiarLinea(linea));
  }

  size_t payloadSize = chunkSize - headerSize;
  size_t contador = 0;
  string buffer;

  for (const auto &registro : registros) {
    if (buffer.size() + registro.size() > payloadSize) {
      // Imprimir chunk con header y padding
      string header = to_string(contador++);
      header = string(4 - header.size(), '0') + header;

      buffer.resize(payloadSize, '@'); // Rellenar con @
      cout << header << buffer << endl;

      buffer = "";
    }
    buffer += registro;
  }

  // Último chunk si quedó algo
  if (!buffer.empty()) {
    string header = to_string(contador++);
    header = string(4 - header.size(), '0') + header;

    buffer.resize(payloadSize, '@');
    cout << header << buffer << endl;
  }
}
int main() {
  ifstream archivo("titanic.csv");
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    return 1;
  }

  size_t chunkSize = 512; // Incluye los 4 del header
  dividirEnChunks(archivo, chunkSize);

  return 0;
}
