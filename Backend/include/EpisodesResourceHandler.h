#pragma once
#include "ResourceHandler.h"

class EpisodesResourceHandler : public ResourceHandler {
public:
  EpisodesResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;

};
