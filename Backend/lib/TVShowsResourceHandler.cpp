#include "TVShowsResourceHandler.h"

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

void MoviesResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into tvshows (show_id,show_name,show_imdbid,show_coverurl) values (1, 'Desperate Housewives', 'tt0410975', 'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2');",errMsg);
    db()->execute("insert into tvshows (show_id,show_name,show_imdbid,show_coverurl) values (2, 'Sex and the City', 'tt0159206', 'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=70729&g2_serialNumber=1');",errMsg);
  }
}
