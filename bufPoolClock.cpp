#include <iostream>
#include <list>
#include <unordered_map>
#include "bufPoolClock.h"

BufPoolC::BufPoolC(int numBuf) : nframe(numBuf) {
  nrequests = 0; 
  nhits = 0;
  nmiss = 0;
}

std::list<int>::iterator BufPoolC::clockAlgorithm(){
  while(true){
    int id = *currentPos;
    Frame2& f = frames[id].first;
    if(f.state){
      f.count--;
      if(f.count <= 0){
        if(f.pin){
          f.count = 0;
        } else {
          f.state = false;
        }
      }
    } else {
      if(f.dirty){
        saveChange(f.id);
        //printf("Guardando Bloque %d...\n", f.id);
      }
      auto victim = currentPos;
      ++currentPos;
      if(currentPos == framesKey.end()) currentPos = framesKey.begin();
      return victim;
    }

    ++currentPos;
    if(currentPos == framesKey.end())
      currentPos = framesKey.begin();    
  }
}

void BufPoolC::saveChange(ssize_t id){
  std::cout<<"Desea guardar los cambios de la pagina "<<id<<"? (Y para confirmar)\n";
  std::string *originalDataBlock = &blocks[id]->getData();
  
  char op;
  std::cin>>op;
  if(op == 'Y' || op == 'y'){
    *originalDataBlock = frames[id].first.data;
    blocks[id]->saveBlock();
  } else {
    std::cout<<"Eliminando cambios en el bloque "<<id<<"...\n";
    frames[id].first.data = *originalDataBlock;
  }
}

std::string& BufPoolC::requestPage(int id, char tipe){ //En megatron, debe de devolver la direccion de memoria del bloque
  nrequests++;
  if(frames.find(id) == frames.end()){
    bool defineTipe = tipe == 'w' ? true : false;
    bool isDirty = defineTipe;
    Frame2 f = {id, isDirty, "", 1, false, defineTipe, true};
    if(framesKey.size() >= nframe){
      auto it = clockAlgorithm();
      int blockId = *it;
      frames.erase(blockId);
      delete blocks[blockId];
      blocks.erase(blockId);
      blocks[id] = new Block(id);
      f.data = blocks[id]->getData();
      frames[id] = std::make_pair(f, it);
      *it = id; 
    } else {
      framesKey.push_back(id);
      auto it = std::prev(framesKey.end());
      blocks[id] = new Block(id);
      f.data = blocks[id]->getData();
      frames[id] = std::make_pair(f, it);
    }
    nmiss++;
  } else {
    Frame2& f = frames[id].first;
    f.count++;
    if(f.tipe == true){
      saveChange(id);
    }

    if(f.count > 0) f.state = true;
    f.tipe = tipe == 'w' ? true : false;

    if(f.dirty == false && f.tipe == true) f.dirty = true;
    nhits++;
  }
  
  if(frames.size() == 1){
    currentPos = framesKey.begin();
  }
  
  return frames[id].first.data;
}

void BufPoolC::pinFrame(int id){
  if(frames.find(id) == frames.end()){
    printf(RED "ERROR: El Frame2 no está cargado\n\n" RESET);
    return;
  }
  frames[id].first.pin = true;
}

void BufPoolC::unPinFrame(int id){
  if(frames.find(id) == frames.end()){
    printf(RED "ERROR: El Frame2 no está cargado\n\n" RESET);
    return;
  }
  frames[id].first.pin = false;
}

void BufPoolC::print(int id, char color){
  printf("FrameID\t│PageID\t│Mode\t│Dirty\t│Count\t│Pin\t│ClockBit\n");
  printf("────────┼───────┼───────┼───────┼───────┼───────┼──────\n");
  int i=0;
  for(std::list<int>::iterator it = framesKey.begin(); it != framesKey.end(); it++){
    auto Frame2 = frames[*it].first;
    char tipe = Frame2.tipe ? 'W' : 'R';
    char dirty = Frame2.dirty ? 'T' : 'F';
    char pin = Frame2.pin ? 'T' : 'F';
    if(id == *it){
      if(color == 'y'){
        printf(YELLOW "%d\t│%d\t│%c\t│%c\t│%d\t│%c\t│%d\n" RESET, i++, Frame2.id, tipe, dirty, Frame2.count, pin, Frame2.state);
      } else {
        printf(GREEN "%d\t│%d\t│%c\t│%c\t│%d\t│%c\t│%d\n" RESET, i++, Frame2.id, tipe, dirty, Frame2.count, pin, Frame2.state);
      }
    } else {
      printf("%d\t│%d\t│%c\t│%c\t│%d\t│%c\t│%d\n", i++, Frame2.id, tipe, dirty, Frame2.count, pin, Frame2.state);
    }
  }
  if(i < nframe){
    for(; i<nframe; i++){
      printf("%d\t│-\t│-\t│-\t│-\t│-\t│-\n", i);
    }
  }
  printf("\n\n");
}

void BufPoolC::printEstadistic(){
  printf(GREEN "=============================================\n");
  printf("Estadisticas\n");
  printf("=============================================\n" RESET);
  printf("N° Hits:\t%d\n", nhits);
  printf("N° Miss:\t%d\n", nmiss);
  printf("Hit Rate:\t%.2f %%\n", (float)nhits * 100.0 / nrequests);
  printf("Miss Rate:\t%.2f %%\n", (float)nmiss * 100.0 / nrequests);
}