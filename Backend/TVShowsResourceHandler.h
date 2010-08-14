#pragma once
#include <pion/net/HTTPServer.hpp>
#include "DB.h"

class TVShowsResourceHandler {
public:
  TVShowsResourceHandler(const DBPtr db);
  void handle(pion::net::HTTPRequestPtr&,pion::net::TCPConnectionPtr&);
private:
  void listTVShows(pion::net::HTTPRequestPtr&,pion::net::TCPConnectionPtr&);
  void findTVShowByID(pion::net::HTTPRequestPtr&,pion::net::TCPConnectionPtr&);

  const DBPtr _db;
};
