#pragma once
#include "MoviesResourceHandler.h"
#include "MovieSourcesResourceHandler.h"
#include "TVShowsResourceHandler.h"
#include "SeasonsResourceHandler.h"
#include "EpisodesResourceHandler.h"
#include "DB.h"

struct Options;

class FrontendServer {
public:
  FrontendServer(const Options&);
  void run();
private:
  void handleNotFound(pion::net::HTTPRequestPtr&,pion::net::TCPConnectionPtr&);

  pion::net::HTTPServer _httpServer;
  const DBPtr _cacheDB;
  MoviesResourceHandler _mh;
  MovieSourcesResourceHandler _msh;
  TVShowsResourceHandler _tvh;
  SeasonsResourceHandler _sh;
  EpisodesResourceHandler _eh;
};
