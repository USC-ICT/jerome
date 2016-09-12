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
#include <jerome/npc/detail/ModelWriterText.hpp>

#include "Split.hpp"

static const char* oInputFile	= "input";
static const char* o1stOutput	= "1st-output";
static const char* o2ndOutput	= "2nd-output";
static const char* oSplit	= "split";
static const char* oSplit_default	= "auto";

Split::Split()
: Command("split", "Split options")
{
  options().add_options()
  (oInputFile, 	po::value<std::string>()->default_value("-"),
   "input file. If none specified, we will use stdin.")
  (o1stOutput,  po::value<std::string>()->default_value("-"),
   "1st output file. If none specified, we will use stdout.")
  (o2ndOutput, 	po::value<std::string>()->default_value("-"),
   "2nd output file. If none specified, we will use stdout.")
  (oSplit, 	po::value<std::string>()->default_value(oSplit_default),
   (std::string("How to split the questions. Provide one of \n")
    + "\"" + std::string(oSplit_default) +"\"    \tdetermine split boundary automatically\n"
    + "<number>  \tuse at most <number> in the first file; place the rest into the second file\n"
    + "<number>% \tuse at most <number> percent of the questions in the first file; place the rest into the second file").c_str())
  ;
}

using namespace jerome;
using namespace jerome::npc;

void Split::run(const po::variables_map& vm) {

  Platform::initialize();
  Platform	p;
  
  // =================================
  // loading a database
  
  {
    auto file = istreamWithName(vm[oInputFile].as<std::string>());
    auto error = p.loadCollection(*file);
    if (error) throw *error;
  }

  
}

std::string Split::description() const
{
  return "split a data file";
}

