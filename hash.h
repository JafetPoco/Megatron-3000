#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <map>
#include <set>
#include <vector>
using namespace std;
class Bucket {
  int depth, size;
  std::map<int, int> values;

public:
  Bucket(int depth, int size);
  int insert(int key, int value);
  int remove(int key);
  int update(int key, int value);
  int search(int key);
  int isFull(void);
  int isEmpty(void);
  int getDepth(void);
  int increaseDepth(void);
  int decreaseDepth(void);
  std::map<int, int> copy(void);
  void clear(void);
  void display(void);
};

class Directory {
  int global_depth, bucket_size;
  std::vector<Bucket *> buckets;
  int hash(int n);
  int pairIndex(int bucket_no, int depth);
  void grow(void);
  void shrink(void);
  void split(int bucket_no);
  void merge(int bucket_no);
  string bucket_id(int n);
  std::string getSerialized() const;

public:
  Directory(int depth, int bucket_size);
  Directory(string file);
  void insert(int key, int value, bool reinserted);
  void remove(int key, int mode);
  void update(int key, int value);
  int search(int key);
  void display(bool duplicates);

  bool readSerialized(const std::string &serialized);
  void persist(const std::string &outname);;
};

void menu();


#endif
