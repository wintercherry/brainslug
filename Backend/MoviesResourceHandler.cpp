#include "MoviesResourceHandler.h"
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/HTTPTypes.hpp>
#include <json/writer.h>
#include <sstream>

namespace {
  void writeJsonHttpResponse(const json::Object& obj,pion::net::HTTPResponseWriter& writer,const bool setStatusOK=true) {
      std::stringstream ss;
      json::Writer::Write(obj,ss);
      if (setStatusOK) {
	writer.getResponse().setStatusCode(pion::net::HTTPTypes::RESPONSE_CODE_OK);
	writer.getResponse().setStatusMessage(pion::net::HTTPTypes::RESPONSE_MESSAGE_OK);
      }
      writer.write(ss.str().c_str());
      writer.send();
  }
}

MoviesResourceHandler::MoviesResourceHandler(const DBPtr db)
  : _db(db) {}

void MoviesResourceHandler::handle(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  if (request->hasQuery("list"))
    listMovies(request,connection);
  else if (request->hasQuery("view"))
    findMovieByID(request,connection);
  else {
    JSONObjectPtr doc(new json::Object);
    (*doc)["error"] = json::String(std::string("unrecognized query ") + request->getQueryString());
    boost::shared_ptr<pion::net::HTTPResponseWriter> writer(
							    pion::net::HTTPResponseWriter::create(
												  connection,
												  *request,
												  boost::bind(&pion::net::TCPConnection::finish, connection)));
    writer->getResponse().setStatusCode(pion::net::HTTPTypes::RESPONSE_CODE_NOT_IMPLEMENTED);
    writer->getResponse().setStatusMessage(pion::net::HTTPTypes::RESPONSE_MESSAGE_NOT_IMPLEMENTED);
    writeJsonHttpResponse(
			  *doc,
			  *writer,
			  false);
  }
}

void MoviesResourceHandler::findMovieByID(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  const pion::net::HTTPTypes::QueryParams& params = request->getQueryParams();
  const pion::net::HTTPTypes::QueryParams::const_iterator match(params.find("view"));
  if (match != params.end()) {
    const std::string& id = match->second;
    if (JSONObjectPtr doc = _db->selectWhere("movies", std::make_pair("id",id))) {
    writeJsonHttpResponse(
			  *doc,
			  *pion::net::HTTPResponseWriter::create(
								 connection,
								 *request,
								 boost::bind(&pion::net::TCPConnection::finish, connection)));
    }
  }
}

void MoviesResourceHandler::listMovies(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  if (JSONObjectPtr doc = _db->select("movies")) {
    writeJsonHttpResponse(
			  *doc,
			  *pion::net::HTTPResponseWriter::create(
								 connection,
								 *request,
								 boost::bind(&pion::net::TCPConnection::finish, connection)));
  }
}
