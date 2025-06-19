#include "recordManager.h"
#include "file.h"
RecordManagerVariable::RecordManagerVariable() { }

void RecordManagerVariable::addToSchema(std::string firstRow, std::string tableName){
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
    tipe = "str";
    size="10";
    // std::cout<<"\tTipo: "; std::cin>>tipe;
    // std::cout<<"\tTamaño: "; std::cin>>size;
    std::string field = word + "#" + tipe + "#" + size + "#";
    schema.write(field);
  }
  std::cout<<"==========================================\n";
}

void RecordManagerVariable::readCSV(std::string file){
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
