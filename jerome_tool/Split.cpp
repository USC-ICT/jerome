//
//  Split.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

