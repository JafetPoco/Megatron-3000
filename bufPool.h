#ifndef BUF_POOL_H
#define BUF_POOL_H
#include <iostream>
#include <list>
#include <unordered_map>

typedef struct Frame {
  int id;
  bool dirty;
  std::string data; //puntero del bloque
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

  std::list<int>::reverse_iterator freeFrame();
public:
  BufPool(int numframe); //constructor
  int requestPage(int id, char tipe);
  void pinFrame(int id);
  void unPinFrame(int id);
  void print();
  void printEstadistic();
};

#endif //buf_pool