#pragma once
#include <Theron/Actor.h>
#include <Theron/ActorRef.h>
#include <boost/filesystem.hpp>
#include <set>
#include "FrontendServerMessages.h"
#include "CommonMessages.h"

namespace bfs = boost::filesystem;

struct StartScanningMessage {
  StartScanningMessage(const bfs::path& path, const std::set<bfs::path::string_type>& supportedExtensions ) : _path(path), _supportedExtensions(supportedExtensions) {}
  const bfs::path _path;
  const std::set<bfs::path::string_type> _supportedExtensions;
};

struct FinishedScanningMessage {};



class FileScanner : public Theron::Actor {
public:
  FileScanner();
  void join();
  void run();
  void addTVShowsSourcePath(const bfs::path& sourcePath);
  void addMoviesSourcePath(const bfs::path& sourcePath);
private:

  void handleFinishedScanningMessage(const FinishedScanningMessage& fsm, const Theron::Address from);

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
  std::vector<Theron::ActorRef> _tvAgents, _movieAgents;
};
