#pragma once
#include "ResourceHandler.h"

class TVShowsResourceHandler : public ResourceHandler {
public:
  TVShowsResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;

};
