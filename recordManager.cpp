#include "recordManager.h"
#include "block.h"
#include "file.h"
#include "globals.h"
#include <fstream>
#include <iostream>

RecordManagerVariable::RecordManagerVariable() {}

std::vector<std::string> splitCSV(const std::string &line) {
  std::vector<std::string> tokens;
  std::string token;
  bool inQuotes = false;

  for (size_t i = 0; i < line.size(); ++i) {
    char c = line[i];

    if (c == '"') {
      inQuotes = !inQuotes; // alternar estado de comillas
    } else if (c == ',' && !inQuotes) {
      tokens.push_back(token);
      token.clear();
    } else {
      token += c;
    }
  }
  tokens.push_back(token); // último token
  return tokens;
}

void RecordManagerVariable::select(std::string tableName) {
  // 1) Cargo el esquema
  sh = new Schema;
  sh->loadFromFile("schemas", tableName);

  // 2) Leo todo el contenido lógico de la tabla
  File table(tableName);
  std::string raw = table.readAll();
  size_t pos = 0;

  // 3) Imprimo cabecera
  for (size_t i = 0; i < sh->getNumFields(); ++i) {
    const Field &f = sh->getField(i);
    std::cout << std::left << std::setw(15) << f.name << "|";
  }
  std::cout << "\n";

  // 4) Recorro cada registro
  size_t regIndex = 0;
  while (pos < raw.size()) {
    std::cout << regIndex++ << "- ";
    size_t startPos = pos;

    try {
      for (size_t i = 0; i < sh->getNumFields(); ++i) {
        const Field &f = sh->getField(i);

        switch (f.type) {
        case INT: {
          std::string s = raw.substr(pos, 8);
          // std::cout << "INT(" << s << ")|";
          int v = std::stoi(s);
          std::cout << std::left << std::setw(15) << v << "|";
          pos += 8;
          break;
        }
        case DOUBLE: {
          std::string s = raw.substr(pos, 8);
          // std::cout << "DBL(" << s << ")|";
          double d = std::stod(s);
          std::cout << std::left << std::setw(15) << d << "|";
          pos += 8;
          break;
        }
        case STRING: {
          std::string lenS = raw.substr(pos, 4);
          // std::cout << "LEN(" << lenS << ")|";
          int len = std::stoi(lenS);
          pos += 4;

          std::string content = raw.substr(pos, len);
          // std::cout << "STR(" << content << ")|";
          std::cout << std::left << std::setw(15) << content << "|";
          pos += len;
          break;
        }
        default:
          throw std::runtime_error("tipo desconocido");
        }
      }

      std::cout << "\n";

    } catch (const std::exception &e) {
      std::cerr << "\n[ERROR] Registro #" << (regIndex - 1)
                << " fallo al parsear el campo #" << ((pos - startPos))
                << "\n  Excepción: " << e.what()
                << "\n  Offset global previo: " << startPos
                << "\n  Posición actual: " << pos << "\n";
      break;
    }
  }
}

void RecordManagerVariable::addToSchema(std::string firstRow,
                                        std::string tableName) {
  std::cout << "==========================================\n";
  std::cout << "Ingrese tamaño y tipo de Dato\n";
  std::cout << "------------------------------------------\n";

  File schema("schemas");
  schema.write(tableName);
  schema.write("#");

  std::string word;
  std::istringstream ss(firstRow);
  while (getline(ss, word, ',')) {
    std::string tipe;
    std::string size;
    std::cout << "Columna: " << word << '\n';
    tipe = "string";
    size = "10";
    // std::cout<<"\tTipo: "; std::cin>>tipe;
    // std::cout<<"\tTamaño: "; std::cin>>size;
    std::string field = word + "#" + tipe + "#" + size + "#";
    schema.write(field);
  }
  std::cout << "==========================================\n";
}

void RecordManagerVariable::readCSV(std::string file) {
  sh = new Schema;
  size_t dotPos = file.find('.');
  std::string tableName = file.substr(0, dotPos);

  std::ifstream csv(file);
  std::string headerTable;
  getline(csv, headerTable);
  addToSchema(headerTable, tableName);

  sh->loadFromFile("schemas", tableName);
  // size_t sizeBlock = disk->info().sectorSize * disk->info().blockLength;
  size_t sizeRegister = sh->getRecordSize();

  File table(tableName);
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(3) << sizeRegister;
  ss << std::setfill('0') << std::setw(4) << -1;
  table.write(ss.str());
  std::string row;
  while (getline(csv, row)) {
    table.write(formatRow(row));
  }
}

std::string RecordManagerVariable::formatRow(std::string row) {
  std::vector<std::string> tokens = splitCSV(row);
  size_t expected = sh->getNumFields();

  if (tokens.size() != expected) {
    std::cerr << "Error: se esperaban " << expected
              << " campos pero se encontraron " << tokens.size() << "\n";
    std::exit(EXIT_FAILURE);
  }

  std::string result;
  result.reserve(row.size() + expected * 8); // estimación

  for (size_t idx = 0; idx < expected; ++idx) {
    const Field &f = sh->getField(idx);
    const std::string &tok = tokens[idx];

    switch (f.type) {
    case INT: {
      int val = tok.empty() ? 0 : std::stoi(tok);
      std::ostringstream os;
      os << std::setw(8) << std::setfill('0') << val;
      result += os.str();
      break;
    }
    case DOUBLE: {
      double val = tok.empty() ? 0.0 : std::stod(tok);
      std::ostringstream os;
      os << std::fixed << std::setprecision(2) << std::setw(8)
         << std::setfill('0') << val;
      std::string s = os.str();
      if (s.size() > 8)
        s = s.substr(0, 8); // por si se pasa
      result += s;
      break;
    }
    case STRING: {
      uint16_t len = static_cast<uint16_t>(tok.size());
      std::ostringstream os;
      os << std::setw(4) << std::setfill('0') << len;
      result += os.str();
      result += tok;
      break;
    }
    default:
      std::cerr << "Tipo de campo no soportado.\n";
      std::exit(EXIT_FAILURE);
    }
  }

  return result;
}

RecordManagerFixed::RecordManagerFixed() {}

void RecordManagerFixed::addToSchema(std::string firstRow,
                                     std::string tableName) {
  std::cout << "==========================================\n";
  std::cout << "Ingrese tamaño y tipo de Dato\n";
  std::cout << "------------------------------------------\n";

  File schema("schema");
  tableName += "#";
  std::string dataSchema = schema.readAll();
  std::cout << "Original: " << dataSchema << "\nSize: " << dataSchema.size()
            << std::endl;

  dataSchema += tableName;
  std::string word;
  std::istringstream ss(firstRow);
  while (getline(ss, word, ',')) {
    std::string tipe;
    std::string size;
    std::cout << "Columna: " << word << '\n';
    std::cout << "\tTipo: ";
    std::cin >> tipe;
    std::cout << "\tTamaño: ";
    std::cin >> size;
    std::string field = word + "#" + tipe + "#" + size + "#";
    dataSchema += field;
  }
  dataSchema += "\n";
  schema.write(dataSchema);
  std::cout << "==========================================\n";
}

std::string RecordManagerFixed::formatRow(std::string row) {
  std::stringstream output;
  std::string value;
  size_t numField = 0;

  bool inQuotes = false;
  std::string token;

  for (size_t i = 0; i <= row.size(); ++i) {
    char c = (i < row.size()) ? row[i] : ','; // Al final, fuerza separación

    if (c == '"') {
      inQuotes = !inQuotes; // Entrar/salir de comillas
    } else if (c == ',' && !inQuotes) {
      // Campo completo
      Field f = sh->getField(numField++);
      output << std::left << std::setfill(' ') << std::setw(f.size) << token;
      token.clear();
    } else {
      token += c;
    }
  }
  return output.str();
}

void RecordManagerFixed::readCSV(std::string file) {
  sh = new Schema;
  size_t dotPos = file.find('.');
  std::string tableName = file.substr(0, dotPos);

  std::ifstream csv(file);
  std::string headerTable;
  getline(csv, headerTable);
  addToSchema(headerTable, tableName);

  sh->loadFromFile("schema", tableName);
  // size_t sizeBlock = disk->info().sectorSize * disk->info().blockLength;
  size_t sizeRegister = sh->getRecordSize();

  File table(tableName);
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(3) << sizeRegister;
  ss << std::setfill('0') << std::setw(4) << -1;
  table.write(ss.str());
  std::string row;
  while (getline(csv, row)) {
    table.write(formatRow(row));
  }
}

void RecordManagerFixed::printHeader(std::string tableName) {
  sh = new Schema;
  sh->loadFromFile("schema", tableName);

  for (size_t i = 0; i < sh->getNumFields(); i++) {
    Field f = sh->getField(i);
    size_t lenField = f.size > f.name.size() ? f.size : f.name.size();
    std::cout << std::left << std::setfill(' ') << std::setw(lenField) << f.name
              << "|";
  }
  std::cout << "\n";
}

void RecordManagerFixed::select(std::string tableName) {
  printHeader(tableName);
}
