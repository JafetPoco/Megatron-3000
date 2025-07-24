#ifndef UPLOAD_CSV_H
#define UPLOAD_CSV_H

#include<iostream>

class IUploadCsv{
public:
  virtual void upload(std::string name) = 0;
  virtual ~IUploadCsv() = default;
};

class UploadCsvFixed : public IUploadCsv{
public:
  void upload(std::string name);
};

#endif