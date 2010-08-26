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

namespace {
  const std::string movie_id("movie_id"); const std::string id("id");
  const std::string movie_name("movie_name"); const std::string name("name");
  const std::string movie_imdbid("movie_imdbid"); const std::string imdbId("imdbId");
  const std::string movie_coverurl("movie_coverurl"); const std::string coverUrl("coverUrl");
}

SanitizedParams MoviesResourceHandler::sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const {
  SanitizedParams sq;
  pion::net::HTTPTypes::QueryParams::const_iterator it(dirtyParams.begin());
  const pion::net::HTTPTypes::QueryParams::const_iterator end(dirtyParams.end());
  for (; it!=end; ++it) {
    std::string sanitizedKey;
    const std::string& key = it->first;
    if (key == id)
      sanitizedKey = movie_id;
    else if (key == name)
      sanitizedKey = movie_name;
    else if (key == coverUrl)
      sanitizedKey = movie_coverurl;
    else if (key == imdbId)
      sanitizedKey = movie_imdbid;
    else
      continue;
    sq.insert(std::make_pair(sanitizedKey,it->second));
  }
  return sq;
}


std::string MoviesResourceHandler::viewStatement() const {
  return "select movie_id as id, movie_name as name, movie_imdbid as imdbId, movie_coverurl as coverUrl from movies";
}

std::string MoviesResourceHandler::listStatement() const {
  return viewStatement() + ";";
}

void MoviesResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into movies(movie_id,movie_name,movie_imdbid,movie_coverurl) values (1, 'Sex and the City', 'tt10000774', 'http://www.pursepage.com/wp-content/uploads/2008/01/sex-and-the-city-movie-poster.jpg');",errMsg);
    db()->execute("insert into movies(movie_id,movie_name,movie_imdbid,movie_coverurl) values (2, 'Twilight', 'tt1099212', 'http://juiceboxdotcom.com/wp-content/themes/mimbo2.2/images//twilight-movie-poster.jpg');",errMsg);
  }
}
