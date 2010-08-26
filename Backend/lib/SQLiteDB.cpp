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

  struct Finalizer {
    Finalizer(sqlite3_stmt* stmt) : _stmt(stmt) {}
    ~Finalizer() {
      sqlite3_finalize(_stmt);
    }
    sqlite3_stmt* const _stmt;
  };
}

bool SQLiteDB::execute(const std::string& query, std::string& errorMessage, const ExecuteCallback& callback, const SanitizedParams& sp) const {
  int ret(-1);
  assert(!query.empty());
  if (sp.empty() ) {
    char* errMsg = 0;
    ret = sqlite3_exec(_db, query.c_str(), execCallback, (void*)&callback, &errMsg);
    if (errMsg) {
      errorMessage = errMsg;
      sqlite3_free(errMsg);
    }
  } else {
    // generate a parameterized where clause for the query
    std::string whereClause(" WHERE ");
    SanitizedParams::const_iterator it(sp.begin());
    const SanitizedParams::const_iterator end(sp.end());
    bool notFirst(false);
    for (; it!=end; ++it) {
      if (notFirst)
	whereClause += " AND ";
      else
	notFirst = true;
      whereClause += it->first + " = ?";
    }
    whereClause += ";";
    const std::string newQuery(query+whereClause);
    // now prep, bind, and step this mofo
    sqlite3_stmt* stmt;
    ret = sqlite3_prepare(_db, newQuery.c_str(), -1, &stmt, 0);
    Finalizer f(stmt);
    if (ret == SQLITE_OK) {
      it = sp.begin();
      int i(1);
      for (; it!=end; ++it) {

	ret = sqlite3_bind_text(stmt,i,it->second.c_str(), it->second.size(), SQLITE_STATIC);
	if (ret != SQLITE_OK)
	  break;
	++i;
      }

      if (ret == SQLITE_OK) {
	std::vector<char*> colNames;
	std::vector<char*> colValues;
	int colCount(-1);
	while (true) {
	  ret = sqlite3_step(stmt);
	  if (ret == SQLITE_ROW) {
	    if (colCount<0L) {
	      // initialize
	      colCount = sqlite3_column_count(stmt);
	      if (colCount==0) {
		callback(colCount,NULL,NULL);
		return true; // empty results. done
	      }
	      for (int i(0); i<colCount; ++i) {
		colNames.push_back((char*)sqlite3_column_name(stmt,i));
	      }
	    }
	    for (int i(0); i<colCount; ++i) {
	      colValues.push_back((char*)sqlite3_column_text(stmt,i));
	    }
	    callback(colCount,
		     &(*colValues.begin()),
		     &(*colNames.begin()));

	  } else
	    break;
	}
      }
    }
  }
  return ret == SQLITE_DONE || ret == SQLITE_OK;
}

JSONObjectPtr SQLiteDB::select(const std::string& fromSource) const {
  return JSONObjectPtr(new json::Object());
}

JSONObjectPtr SQLiteDB::selectWhere(const std::string& fromSource, const std::pair<const std::string,const std::string>& query) const {
  return JSONObjectPtr(new json::Object());
}
