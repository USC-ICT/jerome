//
//  main.cpp
//  jerome_tool
//
//  Created by Anton Leuski on 5/18/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include <iostream>

#include <jerome/npc/npc.hpp>

#include "Evaluate.hpp"
#include "Help.hpp"
#include "Info.hpp"
#include "Split.hpp"
#include "Train.hpp"

int main(int argc, const char * argv[])
{
  Commander::shared().registerClass<Help>();
  Commander::shared().registerClass<Evaluate>();
  Commander::shared().registerClass<Info>();
  Commander::shared().registerClass<Split>();
  Commander::shared().registerClass<Train>();
  
  try {
    jerome::npc::Platform::initialize();
    auto error = Commander::shared().parseArguments(argc, argv);
    if (!error) return 0;
    std::cerr << *error << std::endl;
  } catch (const std::exception& error) {
    std::cerr << error << std::endl;
  }
  Commander::shared().usage(std::cerr);
  return 1;
}
