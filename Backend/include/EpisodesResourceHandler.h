#pragma once
#include "ResourceHandler.h"

class EpisodesResourceHandler : public ResourceHandler {
public:
  EpisodesResourceHandler(const DBPtr db);
  void handle(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);
  virtual void initTestData();
};
