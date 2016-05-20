//
//  main.cpp
//  jerome_tool
//
//  Created by Anton Leuski on 5/18/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace jerome;
using namespace jerome::npc;

static const char* oHelp		= "help";
static const char* oTrainData	= "train-data";
static const char* oDevData		= "dev-data";
static const char* oTestData	= "test-data";
static const char* oDMScript	= "dm";
static const char* oStateName	= "classifier-name";

int main(int argc, const char * argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
  (oHelp, "produce help message")
  (oTrainData, 	po::value<std::string>(), "train data file")
  (oDevData, 		po::value<std::string>(), "dev data file")
  (oTestData, 	po::value<std::string>(), "test data file")
  (oStateName, 	po::value<std::string>(), "classifier name in the data file")
  (oDMScript, 	po::value<std::string>(), "dialogue manager scxml script file")
  ;
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  if (vm.count(oHelp)) {
    std::cout << desc << "\n";
    return 1;
  }

  return 0;
}
