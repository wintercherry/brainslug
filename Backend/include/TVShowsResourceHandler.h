#pragma once
#include "ResourceHandler.h"

class TVShowsResourceHandler : public ResourceHandler {
public:
  TVShowsResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;
  virtual SanitizedParams sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const;
  virtual std::string viewStatement() const;
};
