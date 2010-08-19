#pragma once
#include "DB.h"
#include <sqlite3.h>

class SQLiteDB : public DB {
public:
  SQLiteDB();
  ~SQLiteDB();
  virtual bool execute(const std::string& query, std::string& errorMessage) const;
  virtual JSONObjectPtr select(const std::string& fromSource) const;
  virtual JSONObjectPtr selectWhere(
				    const std::string& fromSource, 
				    const std::pair<const std::string,const std::string>& query) const;

private:
  sqlite3* _db;
};
