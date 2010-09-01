#pragma once
#include <Theron/Actor.h>
#include <boost/filesystem.hpp>
#include <set>
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
  void launchTVAgents();
  void launchMovieAgents();
  void launchTVAgentForPath(const bfs::path& p);
  void launchMovieAgentForPath(const bfs::path& p);
  void stopTVAgents();
  void stopMovieAgents();
  Theron::Address _frontendServer, _joinReceiver;
  std::stack<bfs::path> _tvPaths, _moviePaths;
  std::set<bfs::path::string_type> _supportedExtensions;
};
