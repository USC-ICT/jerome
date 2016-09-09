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

#include "Split.hpp"
#include "Train.hpp"
#include "Help.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace jerome;
using namespace jerome::npc;

int main(int argc, const char * argv[])
{
  Command::registerClass<Help>();
  Command::registerClass<Split>();
  Command::registerClass<Train>();
  
  try {
    Command::parseArguments(argc, argv);
  } catch (const invalid_argument_help& x) {
    std::cerr << x.what() << std::endl;
    Command::usage(std::cerr);
  } catch (const std::exception& x) {
    std::cerr << x.what() << std::endl;
  }
  return 0;
}
