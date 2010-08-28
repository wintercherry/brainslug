#pragma once
#include <Theron/Actor.h>
#include "MoviesResourceHandler.h"
#include "MovieSourcesResourceHandler.h"
#include "TVShowsResourceHandler.h"
#include "SeasonsResourceHandler.h"
#include "EpisodesResourceHandler.h"
#include "DB.h"
#include "CommonMessages.h"
#include "FileScannerMessages.h"
#include "FrontendServerMessages.h"

struct Options;

class FrontendServer : public Theron::Actor {
public:
  FrontendServer(const Options&);
  void run();
  void join();
  typedef Options Parameters;

private:
  void handleNotFound(pion::net::HTTPRequestPtr&,pion::net::TCPConnectionPtr&);
  void handleFileScannerMessage(const FileScannerMessage& fsm, const Theron::Address from);
  void handleJoinMessage(const JoinMessage& jm, const Theron::Address from);
  void handleRunMessage(const RunMessage& rm, const Theron::Address from);

  pion::net::HTTPServer _httpServer;
  const DBPtr _cacheDB;
  MoviesResourceHandler _mh;
  MovieSourcesResourceHandler _msh;
  TVShowsResourceHandler _tvh;
  SeasonsResourceHandler _sh;
  EpisodesResourceHandler _eh;
  Theron::Address _fileScanner;
};
