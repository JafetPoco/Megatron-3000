#include<iostream>
#include<limits>
#include "console.h"

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
  cout<<"(1) Seleccionar Disco"<<endl;
  cout<<"(2) Megatron DBSM"<<endl;
  cout<<"(x) Exit"<<endl;
}

void diskMenu(){
  cout<<"   (1) Crear Disco"<<endl;
  cout<<"   (2) Cargar Disco"<<endl;
  cout<<"   (x) Exit"<<endl;

  while(true){
    char option = getValidatedInput();
    switch (option){
      case '1': cout<<"Creae disco nuevo\n"; option = 'x'; break;
      case '2': cout<<"Cargar disco\n"; option = 'x'; break;
      case 'x': return; break;
      default: cout<<"Opción incorrecta, vuelva a intentar\n";
    }
  }
}

void DBSMMenu(){
  cout<<"hola mundo"<<endl;
}

void console(){
  printHeader();
  printMenu();
  while(true){
    char option = getValidatedInput();

    switch (option){
      case '1': diskMenu(); printMenu(); break;
      case '2': DBSMMenu(); printMenu(); break;
      case 'x': cout << "\nSaliendo del sistema...\n"; return; break;
      default: cout<<"Opción incorrecta, vuelva a intentar\n";
    }
  }
}