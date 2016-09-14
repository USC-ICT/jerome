//
//  ClassifierCommand.cpp
//  jerome
//
//  Created by Anton Leuski on 9/12/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <iostream>
#include <boost/range/adaptors.hpp>

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
