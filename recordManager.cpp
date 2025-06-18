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
  
  File schema("schemas"); 
  schema.write(tableName);

  std::string word;
  std::istringstream ss(firstRow);
  while(getline(ss, word, ',')){
    std::string tipe;
    std::string size;
    std::cout<<"Columna: "<<word<<'\n';
    std::cout<<"\tTipo: "; std::cin>>tipe;
    std::cout<<"\tTamaño: "; std::cin>>size;
    std::string field = word + "#" + tipe + "#" + size + "#";
    schema.write(field);
  }
  std::cout<<"==========================================\n";
}

void RecordManagerFixed::readCSV(std::string file){
  sh = new Schema;
  size_t dotPos = file.find('.');
  std::string tableName = file.substr(0, dotPos);
  std::ifstream csv(file);
  std::string headerTable;
  getline(csv, headerTable);
  addToSchema(headerTable, tableName);
}