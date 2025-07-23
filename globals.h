#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstddef>
class Disk;
class TableFiles;
class FreeBlockManager;
class BufPool;
class SchemaManager;

extern Disk *disk;
extern TableFiles *tableFile;
extern FreeBlockManager *freeBlock;
extern SchemaManager *schemas;
extern BufPool *bufferPool;
extern size_t blockCapacity;

#endif
