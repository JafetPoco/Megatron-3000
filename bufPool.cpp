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
  for(; it != framesKey.rend(); ++it){
    Frame aux = frames[*it].first;
    if(aux.pin == false && aux.dirty == false){
      return it;
    }
  }

  std::cerr<<"ERROR: No es posible agregar una pagina\n";
  return framesKey.rend(); 
}

int BufPool::requestPage(int id, char tipe){ //En megatron, debe de devolver la direccion de memoria del bloque
  printf("Request Page: %d\n", id);

  if(frames.find(id) == frames.end()){
    if(framesKey.size() >= nframe){ //pregunta si hay espacio para agregar una page
      //no hay espacio
      auto posFreeFrame = freeFrame();
      if(posFreeFrame == framesKey.rend()) return -1;
      int lastValue = *posFreeFrame;
      frames.erase(lastValue);
      auto it = framesKey.erase(--posFreeFrame.base());
    }
    framesKey.push_front(id);
    bool mode = tipe == 'r' ? 0 : 1;
    bool dirty = tipe == 'r' ? 0 : 1;
    Frame f = {id, dirty, "", 1, false, mode};
    frames[id] = std::make_pair(f ,framesKey.begin());
    nmiss++;
    nrequests++;
    print();
    return frames[id].first.id;
  } else {

    framesKey.erase(frames[id].second);
    framesKey.push_front(id);
    frames[id].second = framesKey.begin();
    frames[id].first.count++;
    nhits++;

    if(frames[id].first.tipe == true){
      print();
      return frames[id].first.id;
    }
    frames[id].first.dirty = tipe == 'r' ? 0 : 1;
    frames[id].first.tipe = tipe == 'r' ? 0 : 1;

    nrequests++;
    print();
    return frames[id].first.id;
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