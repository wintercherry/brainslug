#pragma once
#include <cstddef> // for size_t etc.
#include <cassert>
#include <map>
#include <string>

#define DEFAULT_PORT 5555
#define DB_CACHE "cache.db"

typedef std::map<std::string,std::string> SanitizedParams;
