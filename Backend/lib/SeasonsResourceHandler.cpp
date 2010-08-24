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

void SeasonsResourceHandler::initTestData() {
  if (boost::dynamic_pointer_cast<SQLiteDB>(db())) {
    std::string errMsg;
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (1,1,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2',1);",errMsg);
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (2,2,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2',1);",errMsg);
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (3,3,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=3',2);",errMsg);
    db()->execute("insert into seasons(season_id,season_number,season_coverurl,tvshow_id) values (4,4,'http://getvideoartwork.com/gallery/main.php?g2_view=core.DownloadItem&g2_itemId=8131&g2_serialNumber=2',2);",errMsg);
  }
}

void SeasonsResourceHandler::handle(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  if (request->hasQuery("tvShow"))
      findByTVShowID(request,connection);
  else
    ResourceHandler::handle(request,connection);
}

namespace {
  struct Lister {
    Lister() : _doc(new json::Object) {}

    void callback(int colCount, char** colValues, char** colNames) {
      json::Object item;
      for (int i(0); i<colCount; ++i) {
	item[colNames[i]] = json::String(colValues[i]);
      }
      _content.Insert(item);
    }

    void finish() {
      (*_doc)["content"] = _content;
    }

    JSONObjectPtr _doc;
    json::Array _content;
  };
}

void SeasonsResourceHandler::list(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  Lister l;
  std::string errMsg;
  if (db()->execute("select * from seasons;",errMsg,boost::bind(&Lister::callback,boost::ref(l),_1,_2,_3))) {
    l.finish();
    writeJsonHttpResponse(
			  *l._doc,
			  *pion::net::HTTPResponseWriter::create(
								 connection,
								 *request,
								 boost::bind(&pion::net::TCPConnection::finish, connection)));
  }
}

void SeasonsResourceHandler::findByTVShowID(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  /*
  const pion::net::HTTPTypes::QueryParams& params = request->getQueryParams();
  const pion::net::HTTPTypes::QueryParams::const_iterator match(params.find("tvShow"));
  if (match != params.end()) {
    const std::string& tvShowID = match->second;
    if (JSONObjectPtr doc = db()->selectWhere(source(), std::make_pair("tvShow",tvShowID))) {
    writeJsonHttpResponse(
			  *doc,
			  *pion::net::HTTPResponseWriter::create(
								 connection,
								 *request,
								 boost::bind(&pion::net::TCPConnection::finish, connection)));
    }
  }
  */
}

