#pragma once
#include "ResourceHandler.h"

class SeasonsResourceHandler : public ResourceHandler {
public:
  SeasonsResourceHandler(const DBPtr db);
  void handle(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);
  virtual void initTestData();
private:
  void findByTVShowID(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);
  void list(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);
};
