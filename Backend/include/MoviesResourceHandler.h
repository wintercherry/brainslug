#pragma once
#include "ResourceHandler.h"

class MoviesResourceHandler : public ResourceHandler {
public:
  MoviesResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;

};
