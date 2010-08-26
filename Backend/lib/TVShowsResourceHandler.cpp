#include "TVShowsResourceHandler.h"
#include "SQLiteDB.h"

TVShowsResourceHandler::TVShowsResourceHandler(const DBPtr db)
  : ResourceHandler(db,"tvshows") {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db)) {
    std::string errMsg;
    if (!db->execute("create table if not exists tvshows (show_id integer primary key asc autoincrement, show_name text not null, show_imdbid text, show_coverurl text);", errMsg)) {
      std::cerr << "Unable to initialize tvshows table in cache db. Reason: " << errMsg << std::endl;
      exit(1);
    }
  }
}

namespace {
  const std::string show_id("show_id"); const std::string id("id");
  const std::string show_name("show_name"); const std::string name("name");
  const std::string show_imdbid("show_imdbid"); const std::string imdbId("imdbId");
  const std::string show_coverurl("show_coverurl"); const std::string coverUrl("coverUrl");
}

SanitizedParams TVShowsResourceHandler::sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const {
  SanitizedParams sq;
  pion::net::HTTPTypes::QueryParams::const_iterator it(dirtyParams.begin());
  const pion::net::HTTPTypes::QueryParams::const_iterator end(dirtyParams.end());
  for (; it!=end; ++it) {
    std::string sanitizedKey;
    const std::string& key = it->first;
    if (key == id)
      sanitizedKey = show_id;
    else if (key == coverUrl)
      sanitizedKey = show_coverurl;
    else if (key == name)
      sanitizedKey = show_name;
    else if (key == imdbId)
      sanitizedKey = show_imdbid;
    else
      continue;
    sq.insert(std::make_pair(sanitizedKey,it->second));
  }
  return sq;
}


std::string TVShowsResourceHandler::viewStatement() const {
  return "select show_id as id, show_imdbid as imdbId, show_name as name, show_coverurl as coverUrl from tvshows";
}

std::string TVShowsResourceHandler::listStatement() const {
  return viewStatement() + ";";
}


void TVShowsResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into tvshows (show_id,show_name,show_imdbid,show_coverurl) values (1, 'Desperate Housewives', 'tt0410975', 'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2');",errMsg);
    db()->execute("insert into tvshows (show_id,show_name,show_imdbid,show_coverurl) values (2, 'Sex and the City', 'tt0159206', 'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=70729&g2_serialNumber=1');",errMsg);
  }
}
