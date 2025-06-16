#ifndef BUF_POOL_H
#define BUF_POOL_H
#include <iostream>
#include <list>
#include <unordered_map>
#include "block.h"

class Block;  

#define WRITE true
#define READ false

typedef struct Frame {
  ssize_t id;
  bool dirty;
  std::string data;
  int count;
  bool pin;
  bool tipe; //Escritura = 1,  lectura = 0
} Frame;


class BufPool{
private:
  int nframe;
  int nrequests;
	int nhits;
	int nmiss;
	int nwrites;

  std::list<int> framesKey;
  std::unordered_map<int, std::pair<Frame, std::list<int>::iterator >> frames;
  std::unordered_map<int, Block*> blocks;

  std::list<int>::reverse_iterator freeFrame();
  void saveChanges(ssize_t id);
public:
  BufPool(int numframe); //constructor
  std::string& requestPage(int id, char tipe);
  void pinFrame(int id);
  void unPinFrame(int id);
  void print();
  void printEstadistic();
  void clearBuffer();
};

#endif //buf_pool