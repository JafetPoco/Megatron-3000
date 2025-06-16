#include <iostream>
#include <list>
#include "bufPool.h"

BufPool::BufPool(int numBuf){
  nframe = numBuf;
  nrequests = 0; 
  nhits = 0;
  nmiss = 0;
  nwrites = 0;
}

std::list<int>::reverse_iterator BufPool::freeFrame(){
  std::list<int>::reverse_iterator it = framesKey.rbegin();
  std::list<int>::reverse_iterator firstDirty = framesKey.rend();

  for(; it != framesKey.rend(); ++it){
    Frame aux = frames[*it].first;
    if(!aux.pin && !aux.dirty){
      return it;
    }

    if(!aux.pin && aux.dirty && firstDirty == framesKey.rend()){
      firstDirty = it;
    }
  }

  if(firstDirty != framesKey.rend()){
    int pos = *firstDirty;
    saveChanges(pos);
    frames[pos].first.dirty = 0;
    return firstDirty;
  }

  std::cerr<<"ERROR: No es posible agregar una pagina\n";
  return framesKey.rend(); 
}

void BufPool::saveChanges(ssize_t id){
  std::cout<<"Desea guardar los cambios de la pagina "<<id<<"? (Y para confirmar)\n";
  std::string *originalDataBlock = &blocks[id]->getData();
  
  char op;
  std::cin>>op;
  if(op == 'Y' || op == 'y'){
    *originalDataBlock = frames[id].first.data;
    //blocks[id]->saveBlock();
  } else {
    std::cout<<"Eliminando cambios en el bloque "<<id<<"...\n";
    frames[id].first.data = *originalDataBlock;
    frames[id].first.dirty = 0;
  }
}

std::string& BufPool::requestPage(int id, char tipe){ //En megatron, debe de devolver la direccion de memoria del bloque
  printf("Request Page: %d\n", id);
  nrequests++;

  if(frames.find(id) == frames.end()){
    if(framesKey.size() >= nframe){ //pregunta si hay espacio para agregar una page
      //no hay espacio
      auto posFreeFrame = freeFrame();
      if(posFreeFrame == framesKey.rend()){
        throw std::runtime_error("No hay espacio disponible :(\n");
      }
      int lastValue = *posFreeFrame;
      frames.erase(lastValue);
      delete blocks[lastValue];
      blocks.erase(lastValue);
      auto it = framesKey.erase(--posFreeFrame.base());
    }
    framesKey.push_front(id);
    blocks[id] = new Block(id);
    std::string dataBlock = blocks[id]->getData();

    bool mode = tipe == 'r' ? 0 : 1;
    bool dirty = tipe == 'r' ? 0 : 1;
    Frame f = {id, dirty, dataBlock, 1, false, mode};
    frames[id] = std::make_pair(f ,framesKey.begin());
    nmiss++;
    print();
    return frames[id].first.data;
  } else {

    nhits++;
    framesKey.erase(frames[id].second);
    framesKey.push_front(id);
    frames[id].second = framesKey.begin();
    frames[id].first.count++;

    if(frames[id].first.tipe == WRITE){
      saveChanges(frames[id].first.id);
      if(tipe == 'w') frames[id].first.dirty = 1;
    } else {
      frames[id].first.dirty = tipe == 'r' ? 0 : 1;
      frames[id].first.tipe = tipe == 'r' ? READ : WRITE;
    }
    print();
    return frames[id].first.data;
  }
}

void BufPool::pinFrame(int id){
  printf("Pin page: %d\n", id);
  auto posFrame = frames.find(id);
  if(posFrame == frames.end()){
    std::cerr<<"ERROR: no se puede pin una página q no está en el buffer\n";
    return;
  }
  
  Frame &f = posFrame->second.first;

  f.pin = true;
  print();
}

void BufPool::unPinFrame(int id){
  printf("Unpin page: %d\n", id);
  auto posFrame = frames.find(id);
  if(posFrame == frames.end()){
    std::cerr<<"ERROR: no se puede unpin una página q no está en el buffer\n";
    return;
  }
  
  Frame &f = posFrame->second.first;

  f.pin = false;
  print();
}

void BufPool::print(){
  printf("FrameID\t|PageID\t|Mode\t|Dirty\t|Count\t|Pin\n");
  printf("---------------------------------------------\n");
  int i=0;
  for(std::list<int>::reverse_iterator it = framesKey.rbegin(); it != framesKey.rend(); it++){
    auto frame = frames[*it].first;
    char tipe = frame.tipe ? 'W' : 'R';
    char dirty = frame.dirty ? 'T' : 'F';
    char pin = frame.pin ? 'T' : 'F';
    printf("%d\t|%d\t|%c\t|%c\t|%d\t|%c\n", i++, frame.id, tipe, dirty, frame.count, pin);
  }
  if(i < nframe){
    for(; i<nframe; i++){
      printf("%d\t|-\t|-\t|-\t|-\t|-\n", i);
    }
  }
  printf("\n\n");
}

void BufPool::printEstadistic(){
  printf("=============================================\n");
  printf("Estadisticas\n");
  printf("=============================================\n");
  printf("N° Requests:\t%d\n", nrequests);
  printf("N° Hits:\t%d\n", nhits);
  printf("N° Miss:\t%d\n", nmiss);
  printf("Hit Rate:\t%.2f %%\n", (float)nhits * 100.0 / nrequests);
  //printf("Hit Rate:\t%.2f %%\n", (float)nreads * 100.0 / nrequests);
}

void BufPool::clearBuffer(){
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
  print();
}