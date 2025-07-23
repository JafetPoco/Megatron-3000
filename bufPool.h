#ifndef BUF_POOL_H
#define BUF_POOL_H
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

typedef struct FrameClock {
  ssize_t id;
  bool dirty;
  std::string data;
  int count;
  bool pin;
  bool tipe; //Escritura = 1,  lectura = 0

  bool state;
} FrameClock;

typedef struct FrameLRU {
  ssize_t id;
  bool dirty;
  std::string data;
  int count;
  bool pin;
  bool tipe; //Escritura = 1,  lectura = 0
} FrameLRU;

class BufPool {
public:
  virtual std::string& requestPage(int id, char tipe) = 0;
  virtual void pinFrame(int id) = 0;
  virtual void unPinFrame(int id) = 0;
  virtual void print(int id, char color) = 0;
  virtual void printEstadistic() = 0;
  virtual void clearBuffer() = 0;
  virtual string type() = 0;
  ~BufPool() = default;
};

class LRU : public BufPool{
private:
  int nframe;
  int nrequests;
	int nhits;
	int nmiss;
	int nwrites;

  std::list<int> framesKey;
  std::unordered_map<int, std::pair<FrameLRU, std::list<int>::iterator >> frames;
  std::unordered_map<int, Block*> blocks;

  std::list<int>::reverse_iterator freeFrame();
  void saveChanges(ssize_t id);
public:
  LRU(int numframe); //constructor
  std::string& requestPage(int id, char tipe);
  void pinFrame(int id);
  void unPinFrame(int id);
  void print(int id, char color);
  void printEstadistic();
  void clearBuffer();
  string type() {return "lru";}
};

class Clock : public BufPool {
private:
  int nframe;
  int nrequests;
	int nhits;
	int nmiss;
  std::list<int>::iterator currentPos;

  std::list<int> framesKey;
  std::unordered_map<int, std::pair<FrameClock, std::list<int>::iterator >> frames;
  std::unordered_map<int, Block*> blocks;

  std::list<int>::iterator clockAlgorithm();
  void saveChange(ssize_t id);
public:
  Clock(int numframe); //constructor
  std::string& requestPage(int id, char tipe);
  void pinFrame(int id);
  void unPinFrame(int id);
  void print(int id, char color);
  void printEstadistic();
  void clearBuffer();
  string type() {return "clock";}
};

#endif //buf_pool
