#ifndef BUF_POOL_CLOCK_H
#define BUF_POOL_CLOCK_H
#include <iostream>
#include <list>
#include <unordered_map>
#include "block.h"

class Block;  

#define WRITE true
#define READ false

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"

typedef struct Frame2 {
  ssize_t id;
  bool dirty;
  std::string data;
  int count;
  bool pin;
  bool tipe; //Escritura = 1,  lectura = 0

  bool state;
} Frame2;


class BufPoolC{
private:
  int nframe;
  int nrequests;
	int nhits;
	int nmiss;
  std::list<int>::iterator currentPos;

  std::list<int> framesKey;
  std::unordered_map<int, std::pair<Frame2, std::list<int>::iterator >> frames;
  std::unordered_map<int, Block*> blocks;

  std::list<int>::iterator clockAlgorithm();
  void saveChange(ssize_t id);
public:
  BufPoolC(int numframe); //constructor
  std::string& requestPage(int id, char tipe);
  void pinFrame(int id);
  void unPinFrame(int id);
  void print(int id, char color);
  void printEstadistic();
};

#endif //buf_pool