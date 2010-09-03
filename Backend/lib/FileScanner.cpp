#include <Theron/Framework.h>
#include <iostream>
#include "FileScanner.h"

FileScanner::FileScanner() 
: _frontendServer(Theron::Address::Null()) {
  RegisterHandler(this, &FileScanner::handleFrontendServerMessage);
  RegisterHandler(this, &FileScanner::handleRunMessage);
  RegisterHandler(this, &FileScanner::handleJoinMessage);
  RegisterHandler(this, &FileScanner::handleFinishedScanningMessage);
  _supportedExtensions.insert(".avi");
  _supportedExtensions.insert(".mp4");
  _supportedExtensions.insert(".mov");
  const bfs::path testPath("/Volumes/Disk1/TV");
  addTVShowsSourcePath(testPath);
}

void FileScanner::addTVShowsSourcePath(const bfs::path& sourcePath) {
  _tvPaths.push(sourcePath);
}

void FileScanner::addMoviesSourcePath(const bfs::path& sourcePath) {
  _moviePaths.push(sourcePath);
}

void FileScanner::run() {
  launchTVAgents();
  launchMovieAgents();
}

void FileScanner::join() {
  stopTVAgents();
  stopMovieAgents();
}

void FileScanner::launchTVAgents() {
   while (!_tvPaths.empty()) {
    const bfs::path& tvPath = _tvPaths.top();
    _tvPaths.pop();
    launchTVAgentForPath(tvPath);
  }
}

namespace {
  const bfs::directory_iterator dirEnd;

  struct TVAgent : public Theron::Actor {
    Theron::Address _fs;
    std::vector<Theron::ActorRef> _tvAgents;

    TVAgent() : _fs(Theron::Address::Null()) {
      RegisterHandler(this, &TVAgent::handle);
    }

    void handle(const StartScanningMessage& sm, const Theron::Address from) {
      _fs = from;
      if (bfs::exists(sm._path)) {
	bfs::directory_iterator dirIt(sm._path);
	for (; dirIt!=dirEnd; ++dirIt) {
	  if (bfs::is_directory(dirIt->status())) {
	    const Theron::ActorRef subDirAgent(GetFramework().CreateActor<TVAgent>());
	    GetFramework().Send(StartScanningMessage(dirIt->path(),sm._supportedExtensions), from, subDirAgent.GetAddress());
	    _tvAgents.push_back(subDirAgent);
	  } else if (sm._supportedExtensions.find(dirIt->path().extension())!=sm._supportedExtensions.end()) {
	    std::cout << "Located matching file: " << dirIt->path().file_string() << std::endl;
	  }
	}
      }   
      GetFramework().Send(FinishedScanningMessage(), GetAddress(), _fs);
    }

  };
}

void FileScanner::launchTVAgentForPath(const bfs::path& p) {
  const Theron::ActorRef agent(GetFramework().CreateActor<TVAgent>());
  GetFramework().Send(StartScanningMessage(p,_supportedExtensions), GetAddress(), agent.GetAddress());
  _tvAgents.push_back(agent);
}

void FileScanner::handleFinishedScanningMessage(const FinishedScanningMessage& fsm, const Theron::Address from) {
  // remove agent
}

void FileScanner::launchMovieAgents() {
  while (!_moviePaths.empty()) {
    const bfs::path& moviePath = _moviePaths.top();
    _moviePaths.pop();
    launchMovieAgentForPath(moviePath);
  }
}

void FileScanner::launchMovieAgentForPath(const bfs::path& p) {
}

void FileScanner::stopTVAgents() {
}

void FileScanner::stopMovieAgents() {
}

void FileScanner::handleFrontendServerMessage(const FrontendServerMessage& fsm, const Theron::Address from) {
  _frontendServer = from;
}

void FileScanner::handleJoinMessage(const JoinMessage& jm, const Theron::Address from) {
  std::cout << "Joining message received" << std::endl;
  join();
  _joinReceiver = from;
  GetFramework().Send(JoinFinishedMessage(), GetAddress(), _joinReceiver);
}

void FileScanner::handleRunMessage(const RunMessage& rm, const Theron::Address from) {
  run();
}
