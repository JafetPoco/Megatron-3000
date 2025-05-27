#include<iostream>
#include<limits>
#include "console.h"
#include "storageManager.h"

using namespace std;

void clearInputBuffer() {
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

char getValidatedInput() {
  char option;
  cout<<"\n->$ ";
  cin >> option;
  clearInputBuffer();
  return tolower(option);
}

void printHeader(){
  cout<<"%\n\t╔═══════════════════════════════════════╗\n";
  cout<<"\t║\tBIENVENIDO A MEGATRON 3000\t║\n";
  cout<<"\t╚═══════════════════════════════════════╝\n\n";
}

void printMenu(){
  cout<<"(1) Crear un nuevo Disco"<<endl;
  cout<<"(2) Disco predefinido (Megatron)"<<endl;
  cout<<"(x) Exit"<<endl;
}

void DBSMMenu(){
  cout<<"hola mundo"<<endl;
}

void initStorage(bool newDisk){
  StorageManager sm;
  sm.initDisckController(newDisk);
}

void console(){
  printHeader();
  printMenu();
  while(true){
    char option = getValidatedInput();
    switch (option){
      case '1': initStorage(true); break;
      case '2': initStorage(false); break;
      case 'x': cout << "\nSaliendo del sistema...\n"; return; break;
      default: cout<<"Opción incorrecta, vuelva a intentar\n";
    }
  }
}