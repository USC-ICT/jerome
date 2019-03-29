//
//  Split.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
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
#include <jerome/npc/detail/ModelReader.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>

#include "Split.hpp"

static const char* oOutputFile      = "output";
static const char* oSplit	          = "split";
static const char* oSplit_default	  = "auto";

po::options_description Split::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  appendInputOptions(options);
  
  options.add_options()
  (oOutputFile,  po::value<std::string>()->default_value("-"),
   "output file. If none specified, we will use stdout.")
  (oSplit, 	po::value<std::string>()->default_value(oSplit_default),
   (std::string("How to split the questions. Provide one of \n")
    + "\"" + std::string(oSplit_default) +"\"    \tdetermine split boundary automatically\n"
    + "<number>  \tuse at most <number> in the first file; place the rest into the second file\n"
    + "<number>% \tuse at most <number> percent of the questions in the first file; place the rest into the second file").c_str())
  ;
  
  return options;
}

using namespace jerome;
using namespace jerome::npc;

OptionalError Split::setup()
{
  return loadCollection();
}

OptionalError Split::teardown()
{
  return platform().saveCollection(*ostreamWithName(variables()[oOutputFile]));
}

OptionalError Split::run1Classifier(const std::string& inName)
{
  return Error::NO_ERROR;
}

std::string Split::description() const
{
  return "split a data file";
}

std::string Split::name() const
{
  return "split";
}

