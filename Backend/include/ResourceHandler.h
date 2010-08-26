#pragma once
#include "DB.h"
#include <pion/net/HTTPServer.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <json/elements.h>

class ResourceHandler {
public:
  ResourceHandler(const DBPtr db, const std::string& source);
  virtual ~ResourceHandler();
  virtual void handle(pion::net::HTTPRequestPtr&,pion::net::TCPConnectionPtr&);
  virtual void initTestData();

protected:
  DBPtr db() const;
  const std::string& source() const;
  virtual SanitizedParams sanitizeQueryParams(const pion::net::HTTPTypes::QueryParams& dirtyParams) const { return SanitizedParams(); }
  virtual std::string listStatement() const { return std::string(); }
  virtual std::string viewStatement() const { return std::string(); } // in derived classes this must return a select statement which allows a where clause to be appended to the end
  virtual void list(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);
  virtual void view(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& connection);
  static void writeJsonHttpResponse(const json::Object& obj, pion::net::HTTPResponseWriter& writer, const bool setStatusOK=true);

private:
  const DBPtr _db;
  const std::string _source;
};
