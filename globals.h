#ifndef GLOBALS_H
#define GLOBALS_H

class Disk;
class TableFiles;
class FreeBlockManager;
class BufPool;

extern Disk *disk;
extern TableFiles *tableFile;
extern FreeBlockManager *freeBlock;
extern BufPool *bufferPool;

#endif