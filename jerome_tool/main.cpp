//
//  main.cpp
//  jerome_tool
//
//  Created by Anton Leuski on 5/18/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
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
