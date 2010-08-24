#pragma once
#include "ResourceHandler.h"

class MovieSourcesResourceHandler : public ResourceHandler {
public:
  MovieSourcesResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;

};
