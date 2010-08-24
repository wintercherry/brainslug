#include "SQLiteDB.h"
#include "Conf.h"

SQLiteDB::SQLiteDB() 
  : _db(0) {
  const int ret(sqlite3_open(DB_CACHE,&_db));
  if (ret) {
    std::cerr << "Couldn't open caching database " << DB_CACHE << ". Reason: " << sqlite3_errmsg(_db) << std::endl;
    exit(1);
  }
}

SQLiteDB::~SQLiteDB() {
  if (_db) {
    sqlite3_close(_db);
  }
}

namespace {
  int execCallback(void* _callback, int colCount, char** colValues, char** colNames) {
    if (_callback) {
      const SQLiteDB::ExecuteCallback& callback = *(const SQLiteDB::ExecuteCallback*)(_callback);
      callback(colCount, colValues, colNames);
    }
    return 0;
  }
}

bool SQLiteDB::execute(const std::string& query, std::string& errorMessage, const ExecuteCallback& callback = ExecuteCallback()) const {
  char* errMsg = 0;
  const int ret( sqlite3_exec(_db, query.c_str(), execCallback, (void*)&callback, &errMsg) );
  if (errMsg) {
    errorMessage = errMsg;
    sqlite3_free(errMsg);
  }
  return ret == SQLITE_OK;
}

JSONObjectPtr SQLiteDB::select(const std::string& fromSource) const {
  return JSONObjectPtr(new json::Object());
}

JSONObjectPtr SQLiteDB::selectWhere(const std::string& fromSource, const std::pair<const std::string,const std::string>& query) const {
  return JSONObjectPtr(new json::Object());
}
