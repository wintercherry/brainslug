#include "FrontendServer.h"
#include "Options.h"
#include "SQLiteDB.h"
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/HTTPTypes.hpp>
#include <boost/bind.hpp>
#include <json/writer.h>
#include <json/elements.h>
#include <iostream>
#include <sstream>
#include <Theron/Framework.h>

FrontendServer::FrontendServer(const Options& o)
  : _httpServer(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), o.port))
  , _cacheDB(new SQLiteDB)
  , _mh(_cacheDB)
  , _msh(_cacheDB)
  , _tvh(_cacheDB)
  , _sh(_cacheDB)
  , _eh(_cacheDB)
  , _fileScanner(Theron::Address::Null()) 
  , _joinReceiver(Theron::Address::Null())
{
  RegisterHandler(this, &FrontendServer::handleFileScannerMessage);
  RegisterHandler(this, &FrontendServer::handleRunMessage);
  RegisterHandler(this, &FrontendServer::handleJoinMessage);
  RegisterHandler(this, &FrontendServer::handleShutdownMessage);
  SetDefaultHandler(this, &FrontendServer::handleDefault);
  _mh.initTestData();
  _msh.initTestData();
  _tvh.initTestData();
  _sh.initTestData();
  _eh.initTestData();
  _httpServer.setLogger(PION_GET_LOGGER("brainslug.HTTPServer"));
  _httpServer.setNotFoundHandler(
				 boost::bind(&FrontendServer::handleNotFound, this, _1, _2));
  _httpServer.addResource(
			  "/movies",
			  boost::bind(&MoviesResourceHandler::handle, &_mh, _1, _2));
  _httpServer.addResource(
			  "/moviesources",
			  boost::bind(&MovieSourcesResourceHandler::handle, &_msh, _1, _2));
  _httpServer.addResource(
			  "/tvshows",
			  boost::bind(&TVShowsResourceHandler::handle, &_tvh, _1, _2));
  _httpServer.addResource(
			  "/seasons",
			  boost::bind(&SeasonsResourceHandler::handle, &_sh, _1, _2));
 _httpServer.addResource(
			  "/episodes",
			  boost::bind(&EpisodesResourceHandler::handle, &_eh, _1, _2));
 _httpServer.addResource(
			  "/shutdown",
			  boost::bind(&FrontendServer::handleShutdown, this, _1, _2));

 // TODO: add http resource handler for adding, listing, removing search paths for filescanner
}

void FrontendServer::join() {
  if (_httpServer.isListening())
    _httpServer.join();
}

void FrontendServer::run() {
  if (!_httpServer.isListening())
    _httpServer.start();
}

namespace {
  void dumpRequestToCout(const pion::net::HTTPRequestPtr request) {
    std::cout << "Request: [method=\"" 
	      << request->getMethod() 
	      << "\"] [resource=\"" 
	      << request->getOriginalResource() 
	      << "\"] [query=\"" 
	      << request->getQueryString() 
	      << "\"]" << std::endl;
  }

}

void FrontendServer::handleNotFound(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  std::cout << "Handled request with no registered service." << std::endl;
  dumpRequestToCout(request);
}

void FrontendServer::stop() {
  _httpServer.stop();
}

void FrontendServer::handleDefault(const Theron::Address from) {
  std::cout << "Received unrecognized message ";
  if (from == _fileScanner)
    std::cout << " from file scanner" << std::endl;
  else if (from == GetAddress())
    std::cout << " from self" << std::endl;
  else if (from == _joinReceiver)
    std::cout << " from join receiver" << std::endl;
  std::cout << std::flush;
}

void FrontendServer::handleShutdown(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection) {
  GetFramework().Send(ShutdownMessage(), GetAddress(), GetAddress());
}

void FrontendServer::handleFileScannerMessage(const FileScannerMessage& fsm, const Theron::Address from) {
  _fileScanner = from;
}

void FrontendServer::handleShutdownMessage(const ShutdownMessage& sm, const Theron::Address from) {
  stop();
  join();
  assert(_joinReceiver!=Theron::Address::Null());
  GetFramework().Send(JoinFinishedMessage(), GetAddress(), _joinReceiver);
}

void FrontendServer::handleJoinMessage(const JoinMessage& jm, const Theron::Address from) {
  _joinReceiver = from;
}

void FrontendServer::handleRunMessage(const RunMessage& rm, const Theron::Address from) {
  run();
}

