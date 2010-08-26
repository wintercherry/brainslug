#pragma once
#include "ResourceHandler.h"

class MoviesResourceHandler : public ResourceHandler {
public:
  MoviesResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual SanitizedParams sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const;
  virtual std::string listStatement() const;
  virtual std::string viewStatement() const;
};
