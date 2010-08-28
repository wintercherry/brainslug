#include "Conf.h"
#include "Options.h"
#include "FrontendServer.h"
#include "FileScanner.h"
#include <Theron/Framework.h>
#include <Theron/Receiver.h>
#include <boost/program_options.hpp>
#include <iostream>

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
      Theron::Framework fw;
      Theron::ActorRef frontendServer(fw.CreateActor<FrontendServer>(o));
      Theron::ActorRef fileScanner(fw.CreateActor<FileScanner>());
      frontendServer.Push(FileScannerMessage(),fileScanner.GetAddress());
      fileScanner.Push(FrontendServerMessage(),frontendServer.GetAddress());
      Theron::Receiver joinReceiver, generalReceiver;
      frontendServer.Push(RunMessage(),generalReceiver.GetAddress());
      fileScanner.Push(RunMessage(),generalReceiver.GetAddress());
      frontendServer.Push(JoinMessage(),joinReceiver.GetAddress());
      joinReceiver.Wait();
      fileScanner.Push(JoinMessage(),joinReceiver.GetAddress());
      joinReceiver.Wait();
    } catch (const std::exception& e) {
      std::cerr << "Caught exception running backend services: " << e.what() << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  startServices(getOptionsFromCommandLine(argc,argv));
}
