#ifndef HASH_H
#define HASH_H

#include <bits/stdc++.h>

using namespace std;
class Bucket {
  int depth, size;
  std::map<int, string> values;

public:
  Bucket(int depth, int size);
  int insert(int key, string value);
  int remove(int key);
  int update(int key, string value);
  void search(int key);
  int isFull(void);
  int isEmpty(void);
  int getDepth(void);
  int increaseDepth(void);
  int decreaseDepth(void);
  std::map<int, string> copy(void);
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

public:
  Directory(int depth, int bucket_size);
  void insert(int key, string value, bool reinserted);
  void remove(int key, int mode);
  void update(int key, string value);
  void search(int key);
  void display(bool duplicates);
};

void menu();


#endif
