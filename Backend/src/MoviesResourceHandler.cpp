#include "MoviesResourceHandler.h"

MoviesResourceHandler::MoviesResourceHandler(const DBPtr db)
  : ResourceHandler(db,"movies") {
  if (db) {
    std::string errMsg;
    if (!db->execute("create table if not exists movies (id integer primary key asc autoincrement, name text not null, imdbid text, coverurl text);", errMsg)) {
      std::cerr << "Unable to initialize movies table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}
