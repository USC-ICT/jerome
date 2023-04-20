//
//  ClassifierCommand.cpp
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

#include <iostream>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <boost/range/adaptors.hpp>
#pragma clang diagnostic pop

#include "ClassifierCommand.hpp"

static const char* oStateName   = "classifier-name";

po::options_description ClassifierCommand::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  options.add_options()
  (oStateName, 	po::value<std::vector<std::string>>()->composing(),
   "classifier names in the data file")
  ;
  
  return options;
}

std::vector<std::string> ClassifierCommand::classifierNames() const
{
  std::vector<std::string>  names;
  if (variables().count(oStateName)) {
    return variables()[oStateName].as<std::vector<std::string>>();
  } else {
    auto range = boost::adaptors::transform(platform().collection().states(),
                                      [](const State& x) { return x.name(); });
    std::vector<std::string>  names(range.begin(), range.end());
    return names;
  }
}

OptionalError ClassifierCommand::run()
{
  auto error = setup();
  if (error) return error;
  
  for(const auto& cn : classifierNames()) {
    auto error = run1Classifier(cn);
    if (error) {
      std::cerr << error->what() << std::endl;
    }
  }
  return teardown();
}

OptionalError ClassifierCommand::setup()
{
  return Error::NO_ERROR;
}

OptionalError ClassifierCommand::teardown()
{
  return Error::NO_ERROR;
}

OptionalError ClassifierCommand::run1Classifier(const std::string& inName)
{
  return Error::NO_ERROR;
}
