#include <Theron/Framework.h>
#include "FileScanner.h"

FileScanner::FileScanner() 
: _frontendServer(Theron::Address::Null()) {
  RegisterHandler(this, &FileScanner::handleFrontendServerMessage);
  RegisterHandler(this, &FileScanner::handleRunMessage);
  RegisterHandler(this, &FileScanner::handleJoinMessage);
}

void FileScanner::run() {
}

void FileScanner::join() {
}

void FileScanner::handleFrontendServerMessage(const FrontendServerMessage& fsm, const Theron::Address from) {
  _frontendServer = from;
}

void FileScanner::handleJoinMessage(const JoinMessage& jm, const Theron::Address from) {
  join();
  GetFramework().Send(JoinFinishedMessage(), from, GetAddress());
}

void FileScanner::handleRunMessage(const RunMessage& rm, const Theron::Address from) {
  run();
}
