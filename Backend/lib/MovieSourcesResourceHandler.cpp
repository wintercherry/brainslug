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

std::string MovieSourcesResourceHandler::listStatement() const {
  return "select msrc_id as id, msrc_url as url, movie_id as movie from moviesources;";
}

void MovieSourcesResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into moviesources(msrc_id,movie_id,msrc_url) values(1, 1, 'http://movies.apple.com/media/us/iphone/2010/ads/apple-iphone4-meet_her-us-20100711_r848-9cie.mov');",errMsg);
    db()->execute("insert into moviesources(msrc_id,movie_id,msrc_url) values(2, 2, 'http://movies.apple.com/media/us/iphone/2010/ads/apple-iphone4-meet_her-us-20100711_r848-9cie.mov');",errMsg);
  }
}
