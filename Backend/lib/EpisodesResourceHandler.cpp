#include "EpisodesResourceHandler.h"
#include "SQLiteDB.h"

EpisodesResourceHandler::EpisodesResourceHandler(const DBPtr db)
  : ResourceHandler(db,"episodes") { 
  if (boost::dynamic_pointer_cast<SQLiteDB>(db)) {
    std::string errMsg;
    if (!db->execute("create table if not exists episodes (episode_id integer primary key asc autoincrement, episode_number integer not null, tvshow_id integer not null, season_id integer not null, episode_name text, episode_imdbid text, episode_imageurl text, foreign key(season_id) references seasons(season_id), foreign key(tvshow_id) references tvshows(tvshow_id), unique (episode_number,season_id,tvshow_id));",errMsg)) {
      std::cerr << "Unable to initialize episodes table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}

namespace {
  const std::string episode_id("episode_id"); const std::string id("id");
  const std::string episode_number("episode_number"); const std::string number("number");
  const std::string tvshow_id("tvshow_id"); const std::string tvShow("tvShow");
  const std::string season_id("season_id"); const std::string season("season");
}

SanitizedParams EpisodesResourceHandler::sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const {
  SanitizedParams sq;
  pion::net::HTTPTypes::QueryParams::const_iterator it(dirtyParams.begin());
  const pion::net::HTTPTypes::QueryParams::const_iterator end(dirtyParams.end());
  for (; it!=end; ++it) {
    std::string sanitizedKey;
    const std::string& key = it->first;
    if (key == id)
      sanitizedKey = episode_id;
    else if (key == number)
      sanitizedKey = episode_number;
    else if (key == tvShow)
      sanitizedKey = tvshow_id;
    else if (key == season)
      sanitizedKey = season_id;
    else
      continue;
    sq.insert(std::make_pair(sanitizedKey,it->second));
  }
  return sq;
}


std::string EpisodesResourceHandler::viewStatement() const {
  return "select episode_id as id, episode_name as name, episode_imdbid as imdbId, episode_number as number, season_id as season, tvshow_id as tvShow from episodes";
}

std::string EpisodesResourceHandler::listStatement() const {
  return viewStatement() + ";";
}

void EpisodesResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into episodes(episode_id,episode_number,episode_name,tvshow_id,season_id) values (1,1,'Pilot',1,1);",errMsg);
    db()->execute("insert into episodes(episode_id,episode_number,episode_name,tvshow_id,season_id) values (2,2,'The story begins!',1,1);",errMsg);
    db()->execute("insert into episodes(episode_id,episode_number,episode_name,tvshow_id,season_id) values (3,1,'The story continues!',2,1);",errMsg);
    db()->execute("insert into episodes(episode_id,episode_number,episode_name,tvshow_id,season_id) values (4,1,'Pilot!',3,2);",errMsg);
  }
}

