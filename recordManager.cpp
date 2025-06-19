#include <iostream>
#include <fstream>
#include "recordManager.h"
#include "globals.h"
#include "file.h"

RecordManagerFixed::RecordManagerFixed() { }

void RecordManagerFixed::addToSchema(std::string firstRow, std::string tableName){
  std::cout<<"==========================================\n";
  std::cout<<"Ingrese tamaño y tipo de Dato\n";
  std::cout<<"------------------------------------------\n";
  
  File schema("schema"); 
  tableName += "#"; 
  std::string dataSchema = schema.readAll();
  std::cout<<"Original: "<<dataSchema<<"\nSize: "<<dataSchema.size()<<std::endl;

  dataSchema += tableName;
  std::string word;
  std::istringstream ss(firstRow);
  while(getline(ss, word, ',')){
    std::string tipe;
    std::string size;
    std::cout<<"Columna: "<<word<<'\n';
    std::cout<<"\tTipo: "; std::cin>>tipe;
    std::cout<<"\tTamaño: "; std::cin>>size;
    std::string field = word + "#" + tipe + "#" + size + "#";
    dataSchema += field;
  }
  dataSchema += "\n";
  schema.write(dataSchema);
  std::cout<<"==========================================\n";
}

std::string RecordManagerFixed::formatRow(std::string row) {
  std::stringstream output;
  std::string value;
  size_t numField = 0;

  bool inQuotes = false;
  std::string token;

  for (size_t i = 0; i <= row.size(); ++i) {
    char c = (i < row.size()) ? row[i] : ',';  // Al final, fuerza separación

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

void RecordManagerFixed::readCSV(std::string file){
  sh = new Schema;
  size_t dotPos = file.find('.');
  std::string tableName = file.substr(0, dotPos);

  std::ifstream csv(file);
  std::string headerTable;
  getline(csv, headerTable);
  addToSchema(headerTable, tableName);

  sh->loadFromFile("schema", tableName);
  //size_t sizeBlock = disk->info().sectorSize * disk->info().blockLength;
  size_t sizeRegister = sh->getRecordSize();

  File table(tableName);
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(3) << sizeRegister;
  ss << std::setfill('0') << std::setw(4) << -1;
  table.write(ss.str());
  std::string row;
  while(getline(csv, row)){
    table.write(formatRow(row));
  }
}

void RecordManagerFixed::printHeader(std::string tableName){
  sh = new Schema;
  sh->loadFromFile("schema", tableName);

  for(size_t i=0; i<sh->getNumFields(); i++){
    Field f = sh->getField(i);
    size_t lenField = f.size > f.name.size() ? f.size : f.name.size();
    std::cout << std::left << std::setfill(' ') << std::setw(lenField) << f.name << "|";
  }
  std::cout<<"\n";
}

void RecordManagerFixed::select(std::string tableName){
  printHeader(tableName);
}

#include "recordManagerVar.cpp"
