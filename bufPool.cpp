#include <iostream>
#include <list>
#include "bufPool.h"

LRU::LRU(int numBuf){
  printf(BLUE "== INICIANDO BUFFER CON LRU ==\n" RESET);
  nframe = numBuf;
  nrequests = 0; 
  nhits = 0;
  nmiss = 0;
  nwrites = 0;
}

std::list<int>::reverse_iterator LRU::freeFrame(){
  std::list<int>::reverse_iterator it = framesKey.rbegin();
  //std::list<int>::reverse_iterator firstDirty = framesKey.rend();

  while(true){
    FrameLRU &aux = frames[*it].first;
    if(aux.count > 1){
      aux.count--;
    } else {
      if(aux.pin == false){
        if(aux.dirty == true)
          saveChanges(aux.id);
        return it;
      }
    }

    it++;
    if(it == framesKey.rend())
      it = framesKey.rbegin();
  }

  std::cerr<<"ERROR: No es posible agregar una pagina\n";
  return framesKey.rend(); 
}

void LRU::saveChanges(ssize_t id){
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
    frames[id].first.dirty = 0;
  }
}

std::string& LRU::requestPage(int id, char tipe){ //En megatron, debe de devolver la direccion de memoria del bloque
  //printf("Request Page: %d\n", id);
  nrequests++;
  std::cerr<<"BUFFER: solicitando pagina "<<id<<'\n';

  if(frames.find(id) == frames.end()){
    if(framesKey.size() >= (size_t) nframe){ //pregunta si hay espacio para agregar una page
      //no hay espacio
      auto posFreeFrame = freeFrame();
      if(posFreeFrame == framesKey.rend()){
        throw std::runtime_error("No hay espacio disponible :(\n");
      }
      int lastValue = *posFreeFrame;
      frames.erase(lastValue);
      delete blocks[lastValue];
      blocks.erase(lastValue);
      framesKey.erase(--posFreeFrame.base());
    }
    framesKey.push_front(id);
    blocks[id] = new Block(id);
    std::string dataBlock = blocks[id]->getData();

    bool mode = tipe == 'r' ? READ : WRITE;
    bool dirty = tipe == 'r' ? 0 : 1;
    FrameLRU f = {id, dirty, dataBlock, 1, false, mode};
    frames[id] = std::make_pair(f ,framesKey.begin());
    nmiss++;
    //print();
    return frames[id].first.data;
  } else {
    nhits++;
    framesKey.erase(frames[id].second);
    framesKey.push_front(id);
    frames[id].second = framesKey.begin();
    frames[id].first.count++;

    if(frames[id].first.tipe == WRITE){
      saveChanges(frames[id].first.id);
      //if(tipe == 'w') frames[id].first.dirty = 1; //OJITO AQUI
    } else {
      frames[id].first.tipe = tipe == 'r' ? READ : WRITE;
      frames[id].first.dirty = tipe == 'r' ? false : true;
    }
    //print();
    return frames[id].first.data;
  }
}

void LRU::pinFrame(int id){
  printf("Pin page: %d\n", id);
  auto posFrame = frames.find(id);
  if(posFrame == frames.end()){
    std::cerr<<"ERROR: no se puede pin una página q no está en el buffer\n";
    return;
  }
  
  FrameLRU &f = posFrame->second.first;

  f.pin = true;
}

void LRU::unPinFrame(int id){
  printf("Unpin page: %d\n", id);
  auto posFrame = frames.find(id);
  if(posFrame == frames.end()){
    std::cerr<<"ERROR: no se puede unpin una página q no está en el buffer\n";
    return;
  }
  
  FrameLRU &f = posFrame->second.first;

  f.pin = false;
}

void LRU::print(int id, char color){
  printf("FrameID\t|PageID\t|Mode\t|Dirty\t|Count\t|Pin\n");
  printf("---------------------------------------------\n");
  int i=0;
  for(std::list<int>::reverse_iterator it = framesKey.rbegin(); it != framesKey.rend(); it++){
    auto frame = frames[*it].first;
    char tipe = frame.tipe ? 'W' : 'R';
    char dirty = frame.dirty ? 'T' : 'F';
    char pin = frame.pin ? 'T' : 'F';
    if(id == *it){
      if(color == 'y'){
        printf(YELLOW "%d\t|%zd\t|%c\t|%c\t|%d\t|%c\n" RESET, i++, frame.id, tipe, dirty, frame.count, pin);
      } else {
        printf(GREEN "%d\t|%zd\t|%c\t|%c\t|%d\t|%c\n" RESET, i++, frame.id, tipe, dirty, frame.count, pin);
      }
    } else {
      printf("%d\t|%zd\t|%c\t|%c\t|%d\t|%c\n", i++, frame.id, tipe, dirty, frame.count, pin);
    }
  }
  if(i < nframe){
    for(; i<nframe; i++){
      printf("%d\t|-\t|-\t|-\t|-\t|-\n", i);
    }
  }
  printf("\n\n");
}

void LRU::printEstadistic(){
  printf("=============================================\n");
  printf("Estadisticas\n");
  printf("=============================================\n");
  printf("N° Requests:\t%d\n", nrequests);
  printf("N° Hits:\t%d\n", nhits);
  printf("N° Miss:\t%d\n", nmiss);
  printf("Hit Rate:\t%.2f %%\n", (float)nhits * 100.0 / nrequests);
  //printf("Hit Rate:\t%.2f %%\n", (float)nreads * 100.0 / nrequests);
}

void LRU::clearBuffer(){
  for(std::list<int>::iterator it = framesKey.begin(); it != framesKey.end(); it++){
    if(frames[*it].first.dirty == 1){
      std::string *originalData = &blocks[*it]->getData();
      *originalData = frames[*it].first.data;
      blocks[*it]->saveBlock();
    }
    delete blocks[*it];
    blocks.erase(*it);
    frames.erase(*it);
  }
}

// Clock

Clock::Clock(int numBuf) : nframe(numBuf) {
  printf(BLUE "== INICIANDO BUFFER CON CLOCK ==\n" RESET);
  nrequests = 0; 
  nhits = 0;
  nmiss = 0;
}

std::list<int>::iterator Clock::clockAlgorithm(){
  while(true){
    int id = *currentPos;
    FrameClock& f = frames[id].first;
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

void Clock::saveChange(ssize_t id){
  // std::cout<<"Desea guardar los cambios de la pagina "<<id<<"? (Y para confirmar)\n";
  std::string *originalDataBlock = &blocks[id]->getData();
  
  char op = 'y';
  // std::cin>>op;
  if(op == 'Y' || op == 'y'){
    *originalDataBlock = frames[id].first.data;
    blocks[id]->saveBlock();
  } else {
    std::cout<<"Eliminando cambios en el bloque "<<id<<"...\n";
    frames[id].first.data = *originalDataBlock;
  }
}

std::string& Clock::requestPage(int id, char tipe){ //En megatron, debe de devolver la direccion de memoria del bloque
  nrequests++;
  if(frames.find(id) == frames.end()){
    bool defineTipe = tipe == 'w' ? true : false;
    bool isDirty = defineTipe;
    FrameClock f = {id, isDirty, "", 1, false, defineTipe, true};
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
    FrameClock& f = frames[id].first;
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

void Clock::pinFrame(int id){
  if(frames.find(id) == frames.end()){
    printf(RED "ERROR: El FrameClock no está cargado\n\n" RESET);
    return;
  }
  frames[id].first.pin = true;
}

void Clock::unPinFrame(int id){
  if(frames.find(id) == frames.end()){
    printf(RED "ERROR: El FrameClock no está cargado\n\n" RESET);
    return;
  }
  frames[id].first.pin = false;
}

void Clock::print(int id, char color){
  printf("FrameID\t│PageID\t│Mode\t│Dirty\t│Count\t│Pin\t│ClockBit\n");
  printf("────────┼───────┼───────┼───────┼───────┼───────┼──────\n");
  int i=0;
  for(std::list<int>::iterator it = framesKey.begin(); it != framesKey.end(); it++){
    auto FrameClock = frames[*it].first;
    char tipe = FrameClock.tipe ? 'W' : 'R';
    char dirty = FrameClock.dirty ? 'T' : 'F';
    char pin = FrameClock.pin ? 'T' : 'F';
    if(id == *it){
      if(color == 'y'){
        printf(YELLOW "%d\t│%zd\t│%c\t│%c\t│%d\t│%c\t│%d\n" RESET, i++, FrameClock.id, tipe, dirty, FrameClock.count, pin, FrameClock.state);
      } else {
        printf(GREEN "%d\t│%zd\t│%c\t│%c\t│%d\t│%c\t│%d\n" RESET, i++, FrameClock.id, tipe, dirty, FrameClock.count, pin, FrameClock.state);
      }
    } else {
      printf("%d\t│%zd\t│%c\t│%c\t│%d\t│%c\t│%d\n", i++, FrameClock.id, tipe, dirty, FrameClock.count, pin, FrameClock.state);
    }
  }
  if(i < nframe){
    for(; i<nframe; i++){
      printf("%d\t│-\t│-\t│-\t│-\t│-\t│-\n", i);
    }
  }
  printf("\n\n");
}

void Clock::printEstadistic(){
  printf(GREEN "=============================================\n");
  printf("Estadisticas\n");
  printf("=============================================\n" RESET);
  printf("N° Hits:\t%d\n", nhits);
  printf("N° Miss:\t%d\n", nmiss);
  printf("Hit Rate:\t%.2f %%\n", (float)nhits * 100.0 / nrequests);
  printf("Miss Rate:\t%.2f %%\n", (float)nmiss * 100.0 / nrequests);
}

void Clock::clearBuffer(){
  for(std::list<int>::iterator it = framesKey.begin(); it != framesKey.end(); it++){
    if(frames[*it].first.dirty == 1){
      std::string *originalData = &blocks[*it]->getData();
      *originalData = frames[*it].first.data;
      blocks[*it]->saveBlock();
    }
    delete blocks[*it];
    blocks.erase(*it);
    frames.erase(*it);
  }
}
