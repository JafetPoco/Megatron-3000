#ifndef MENU_H
#define MENU_H
#include "disk.h"
#include "globals.h"

void menu();
void menu_disk();
void menu_buffer();
Disk* menu_disk_create();
Disk* menu_disk_open();

void menu_megatron();
void menu_select();
void menu_where();
void menu_insert();
void menu_readCSV();
//debug
void menu_disk_debug();
#endif //MENU_H
