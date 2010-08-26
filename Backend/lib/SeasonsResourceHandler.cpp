#include "SeasonsResourceHandler.h"
#include "SQLiteDB.h"

SeasonsResourceHandler::SeasonsResourceHandler(const DBPtr db)
  : ResourceHandler(db,"seasons") { 
  if (boost::dynamic_pointer_cast<SQLiteDB>(db)) {
    std::string errMsg;
    if (!db->execute("create table if not exists seasons (season_id integer primary key asc autoincrement, season_number integer not null, season_coverurl text, tvshow_id integer not null, foreign key(tvshow_id) references tvshows(show_id), unique (season_number,tvshow_id));",errMsg)) {
      std::cerr << "Unable to initialize seasons table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}

namespace {
  const std::string season_id("season_id"); const std::string id("id");
  const std::string season_number("season_number"); const std::string number("number");
  const std::string season_coverurl("season_coverurl"); const std::string coverUrl("coverUrl");
  const std::string tvshow_id("tvshow_id"); const std::string tvShow("tvShow");
}

SanitizedParams SeasonsResourceHandler::sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const {
  SanitizedParams sq;
  pion::net::HTTPTypes::QueryParams::const_iterator it(dirtyParams.begin());
  const pion::net::HTTPTypes::QueryParams::const_iterator end(dirtyParams.end());
  for (; it!=end; ++it) {
    std::string sanitizedKey;
    const std::string& key = it->first;
    if (key == id)
      sanitizedKey = season_id;
    else if (key == number)
      sanitizedKey = season_number;
    else if (key == coverUrl)
      sanitizedKey = season_coverurl;
    else if (key == tvShow)
      sanitizedKey = tvshow_id;
    else
      continue;
    sq.insert(std::make_pair(sanitizedKey,it->second));
  }
  return sq;
}

void SeasonsResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (1,1,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2',1);",errMsg);
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (2,2,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2',1);",errMsg);
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (3,3,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=3',2);",errMsg);
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (4,4,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2',2);",errMsg);
  }
}

std::string SeasonsResourceHandler::viewStatement() const {
  return "select season_id as id, season_number as number, season_coverurl as coverUrl, tvshow_id as tvShow from seasons";
}

std::string SeasonsResourceHandler::listStatement() const {
  return viewStatement() + ";";
}


