#pragma once
#include "ResourceHandler.h"

class MovieSourcesResourceHandler : public ResourceHandler {
public:
  MovieSourcesResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;
  virtual SanitizedParams sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const;
  virtual std::string viewStatement() const;

};
