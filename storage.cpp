#include "storage.h"
#include "globals.h"
#include "recordManager.h"
#include "schema.h"

bool storageManager::uploadCSV(string csvfile, string tableName) {
  try {
    schemas->uploadCsv(csvfile, tableName);
    RecordManagerFixed rm(tableName);

    CSVProcessor csv(csvfile);
    vector<Record> test = csv.getData();

    Schema schm = schemas->getSchema(tableName);
    auto formatted = rm.formatRows(test, schm);
    rm.write(formatted);

    return true;
  } catch (const std::exception& e) {
    std::cerr << "storageManager::uploadCSV - Error: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "storageManager::uploadCSV - Error desconocido\n";
    return false;
  }
}
