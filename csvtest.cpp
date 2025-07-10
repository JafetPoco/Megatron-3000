#include <fstream>
#include "disk.h"
#include "block.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

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

int main() {
  ifstream archivo("titanic.csv");
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    return 1;
  }

  size_t chunkSize = 512;
  vector<string> chunks = chunkSplit(archivo, chunkSize);

  cout << "Chunks generados: " << chunks.size() << endl;
  if (!chunks.empty()) {
    for (auto& i: chunks) {
      cout<<i<<"\n\n";
    }
  }
  return 0;
}
