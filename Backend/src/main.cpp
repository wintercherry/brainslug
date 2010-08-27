#include "Conf.h"
#include "Options.h"
#include "FrontendServer.h"
#include "FileScanner.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <Theron/Framework.h>

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
      FrontendServer frontendServer(o);
      FileScanner fileScanner;
      {
        frontendServer.run();
        fileScanner.run();
	// always join the filescanner before the frontendserver!
	fileScanner.join();
	frontendServer.join();
      }
    } catch (const std::exception& e) {
      std::cerr << "Caught exception running backend services: " << e.what() << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  startServices(getOptionsFromCommandLine(argc,argv));
}
