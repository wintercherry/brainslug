#pragma once
#include "ResourceHandler.h"

class EpisodesResourceHandler : public ResourceHandler {
public:
  EpisodesResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;
  virtual SanitizedParams sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const;
  virtual std::string viewStatement() const;
};
