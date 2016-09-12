//
//  main.cpp
//  jerome_tool
//
//  Created by Anton Leuski on 5/18/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include <iostream>

#include "Split.hpp"
#include "Evaluate.hpp"
#include "Train.hpp"
#include "Help.hpp"

int main(int argc, const char * argv[])
{
  Command::registerClass<Help>();
  Command::registerClass<Split>();
  Command::registerClass<Evaluate>();
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
