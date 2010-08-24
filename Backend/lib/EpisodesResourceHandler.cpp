#include "EpisodesResourceHandler.h"
#include "SQLiteDB.h"

EpisodesResourceHandler::EpisodesResourceHandler(const DBPtr db)
  : ResourceHandler(db,"episodes") { 
  if (boost::dynamic_pointer_cast<SQLiteDB>(db)) {
    std::string errMsg;
    if (!db->execute("create table if not exists episodes (episode_id integer primary key asc autoincrement, episode_number integer not null, tvshow_id integer not null, season_id integer not null, episode_name text, episode_imdbid text, episode_imageurl text, foreign key(season_id) references seasons(season_id), foreign key(tvshow_id) references tvshows(tvshow_id), unique (episode_number,season_id));",errMsg)) {
      std::cerr << "Unable to initialize episodes table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}

void EpisodesResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into episodes(episode_id,episode_number,episode_name,tvshow_id,season_id) values (1,1,'Pilot',1,1);",errMsg);
    db()->execute("insert into episodes(episode_id,episode_number,episode_name,tvshow_id,season_id) values (2,2,'The story begins!',1,1);",errMsg);
    db()->execute("insert into episodes(episode_id,episode_number,tvshow_id,season_id) values (2,2,'The store continues!',1,1);",errMsg);
    db()->execute("insert into episodes(episode_id,episode_number,tvshow_id,season_id) values (4,1,'Pilot!'2,3);",errMsg);
  }
}

void EpisodesResourceHandler::handle(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
}
