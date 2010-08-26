#pragma once
#include "ResourceHandler.h"

class SeasonsResourceHandler : public ResourceHandler {
public:
  SeasonsResourceHandler(const DBPtr db);
  virtual void initTestData();
protected:
  virtual std::string listStatement() const;
  virtual std::string viewStatement() const;
  virtual SanitizedParams sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const;
};
