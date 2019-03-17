//
//  PlatformCommand.cpp
//  jerome
//
//  Created by Anton Leuski on 9/12/16.
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

#include <jerome/npc/detail/ModelReader.hpp>

#include "PlatformCommand.hpp"

static const char* oInputFile       = "input";
static const char* oInputFileFormat = "input-format";

PlatformCommand::PlatformCommand()
{
}

OptionalError 
PlatformCommand::loadCollection(const po::variable_value& inName, 
                                const po::variable_value& inFormat)
{
  OptionalString format;
  if (!inFormat.empty()) {
    format = inFormat.as<std::string>();
  } else {
    if (!inName.empty()) {
      auto name = inName.as<std::string>();
      format = ModelReaderFactory::sharedInstance().providerIDForFileName(name);
    }
  }
  return platform().loadCollection(*istreamWithName(inName));
}

OptionalError 
PlatformCommand::loadCollection()
{
  return loadCollection(variables()[oInputFile], variables()[oInputFileFormat]);
}

void PlatformCommand::appendInputOptions(po::options_description& ioOptions) const
{
  ioOptions.add_options()
  (oInputFile,   po::value<std::string>()->default_value("-"),
   "input file (default '-': standard input)")
  (oInputFileFormat, po::value<std::string>(),
   (std::string("input file format ")
    + "(default: guess from the file extension, otherwise use xml). "
    + "You may provide one of\n  "
    + boost::algorithm::join(ModelReaderFactory::sharedInstance().providerIDs(), "\n  ")
    ).c_str())
  ;
}

std::string PlatformCommand::inputFileName(const po::variables_map& inVM) const 
{
  return inVM[oInputFile].as<std::string>();
}
