#include "Conf.h"
#include "Options.h"
#include "FrontendServer.h"
#include "FileScanner.h"
#include <Theron/Framework.h>
#include <Theron/Receiver.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <cassert>

namespace {

  Options getOptionsFromCommandLine(const int argc, char* argv[]) {
    Options o;
    namespace po = boost::program_options;
    po::options_description od("Server options");
    od.add_options()
      ("help,h", "displays this help message")
      ("port,p", 
       po::value<size_t>(&o.port)->default_value(DEFAULT_PORT), 
       "what port the backend should listen on");
    po::variables_map vm;
    po::store(
	      po::parse_command_line(
				     argc, 
				     argv, 
				     od), 
	      vm);
    po::notify(vm);
    if (vm.count("help")) {
      std::cout << od << std::endl;
      exit(1);
    }
    return o;
  }

  void startServices(const Options& o) {
    try {
      Theron::Framework fw(2);
      Theron::ActorRef frontendServer(fw.CreateActor<FrontendServer>(o));
      Theron::ActorRef fileScanner(fw.CreateActor<FileScanner>());
      frontendServer.Push(FileScannerMessage(),fileScanner.GetAddress());
      fileScanner.Push(FrontendServerMessage(),frontendServer.GetAddress());
      Theron::Receiver frontendReceiver, fileScannerReceiver;
      frontendServer.Push(RunMessage(),frontendReceiver.GetAddress());
      fileScanner.Push(RunMessage(),fileScannerReceiver.GetAddress());
      frontendServer.Push(JoinMessage(),frontendReceiver.GetAddress());
      assert(frontendReceiver.Count()==0);
      frontendReceiver.Wait();
      fileScanner.Push(JoinMessage(),fileScannerReceiver.GetAddress());
      fileScannerReceiver.Wait();
    } catch (const std::exception& e) {
      std::cerr << "Caught exception running backend services: " << e.what() << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  startServices(getOptionsFromCommandLine(argc,argv));
}
