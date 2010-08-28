#pragma once
#include <Theron/Actor.h>
#include <boost/filesystem.hpp>
#include "FrontendServerMessages.h"
#include "CommonMessages.h"

namespace bfs = boost::filesystem;

class FileScanner : public Theron::Actor {
public:
  FileScanner();
  void join();
  void run();
  void addTVShowsSourcePath(const bfs::path& sourcePath);
  void addMoviesSourcePath(const bfs::path& sourcePath);
private:
  void handleFrontendServerMessage(const FrontendServerMessage& fsm, const Theron::Address from);
  void handleRunMessage(const RunMessage& rm, const Theron::Address from);
  void handleJoinMessage(const JoinMessage& rm, const Theron::Address from);
  Theron::Address _frontendServer;
};
