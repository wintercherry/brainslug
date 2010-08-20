#include "MoviesResourceHandler.h"
#include "SQLiteDB.h"

MoviesResourceHandler::MoviesResourceHandler(const DBPtr db)
  : ResourceHandler(db,"movies") {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db)) {
    std::string errMsg;
    if (!db->execute("create table if not exists movies (movie_id integer primary key asc autoincrement, movie_name text not null, movie_imdbid text, movie_coverurl text);", errMsg)) {
      std::cerr << "Unable to initialize movies table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}

void MoviesResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into movies(movie_id,movie_name,movie_imdbid,movie_coverurl) values (1, 'Sex and the City', 'tt10000774', 'http://www.pursepage.com/wp-content/uploads/2008/01/sex-and-the-city-movie-poster.jpg');",errMsg);
    db()->execute("insert into movies(movie_id,movie_name,movie_imdbid,movie_coverurl) values (2, 'Twilight', 'tt1099212', 'http://juiceboxdotcom.com/wp-content/themes/mimbo2.2/images//twilight-movie-poster.jpg');",errMsg);
  }
}
