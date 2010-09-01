#include <Theron/Framework.h>
#include <iostream>
#include "FileScanner.h"

FileScanner::FileScanner() 
: _frontendServer(Theron::Address::Null()) {
  RegisterHandler(this, &FileScanner::handleFrontendServerMessage);
  RegisterHandler(this, &FileScanner::handleRunMessage);
  RegisterHandler(this, &FileScanner::handleJoinMessage);
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
}

void FileScanner::launchTVAgentForPath(const bfs::path& p) {
  if (bfs::exists(p)) {
    bfs::directory_iterator dirIt(p);
    for (; dirIt!=dirEnd; ++dirIt) {
      if (bfs::is_directory(dirIt->status())) {
	launchTVAgentForPath(dirIt->path());
      } else if (_supportedExtensions.find(dirIt->path().extension())!=_supportedExtensions.end()) {
	std::cout << "Located matching file: " << dirIt->path().file_string() << std::endl;
      }
    }
  }
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
