#ifndef DISK_CONTROLER_H
#define DISK_CONTROLER_H
#include "disk.h"

class DiskController{
  private:
    Disk* disco;
    char** buffer; 
  public:
    bool existDisk();
    void initDisk(bool newDisk);
    std::string getAddress(int numBlock) const;
};

#endif