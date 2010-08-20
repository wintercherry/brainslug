#include "MovieSourcesResourceHandler.h"
#include "SQLiteDB.h"

MovieSourcesResourceHandler::MovieSourcesResourceHandler(const DBPtr db)
  : ResourceHandler(db,"movieSources") {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db)) {
    std::string errMsg;
    if (!db->execute("create table if not exists moviesources (msrc_id integer primary key asc autoincrement, movie_id integer not null, msrc_url text, foreign key(movie_id) references movies(movie_id));", errMsg)) {
      std::cerr << "Unable to initialize movie_sources table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}

void MovieSourcesResourceHandler::initTestData() {

}
