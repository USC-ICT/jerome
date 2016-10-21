//
//  Info.cpp
//  jerome
//
//  Created by Anton Leuski on 9/20/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include <iostream>

#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>
#include <jerome/type/algorithm.hpp>

#include "Info.hpp"

static const char* oInputFile     = "input";
static const char* oOutputFile    = "output";
static const char* oVerbosity     = "verbosity";

po::options_description Info::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  options.add_options()
  (oVerbosity, 	po::value<int>()->default_value(0),
   "verbosity level")
  (oInputFile, 	po::value<std::string>()->default_value("-"),
   "input file. '-' = stdin.")
  (oOutputFile, 	po::value<std::string>()->default_value("-"),
   "output file. '-' = stdout")
  ;
  
  return options;
}

using namespace jerome;
using namespace jerome::npc;

OptionalError Info::setup()
{
  mOutput = ostreamWithName(variables()[oOutputFile]);
  return platform().loadCollection(*istreamWithName(variables()[oInputFile]));
}

//OptionalError Info::teardown()
//{
//  return platform().saveCollection(*ostreamWithName(variables()[oOutputFile]));
//}

template <typename T>
static void printFieldInfo(std::ostream& os, const T& range,
                           const po::variables_map& inVM)
{
  typedef StringMap<unsigned> field_type;
  typedef std::pair<String,unsigned> pair_type;
  
  StringMap<field_type> fields;
  for(const auto& u : range) {
    for(const auto& fn : u.fieldNames()) {
      String fv = u.get(fn);
      fields[fn][fv] += 1;
    }
  }
  auto fieldKeys = keys(fields);
  String tab = "  ";
  std::sort(fieldKeys.begin(), fieldKeys.end());
  
  int verbosity = inVM[oVerbosity].as<int>();
  
  for(const auto& key : fieldKeys) {
    const auto& field = fields[key];
    os  << tab << std::setw(12) << std::left << key
        << " " << field.size() << std::endl;
    // each level of verbosity adds another 10 to the size
    // of the categories that are printed out. Categories with 
    // size less than 10*verbosity only printed as names
    if (field.size() > 10*verbosity) continue;
    if (verbosity <= 1) continue;
    List<pair_type> values(field.begin(), field.end());
    std::sort(values.begin(), values.end(),
              [](const pair_type& a, const pair_type& b)
              { return b.second < a.second; });
    for(const auto& v : values) {
      os << tab << tab << std::setw(12) << v.first
        << ": " << v.second << std::endl;
    }
  }
}

OptionalError Info::run1Classifier(const std::string& inStateName)
{
  auto optState = platform().collection().states()
  .optionalObjectWithName(inStateName);
  if (!optState) {
    return Error(std::string("Invalid state name: ") + inStateName);
  }

  *mOutput << "state: " << inStateName << std::endl;
  *mOutput << "# questions: " << optState->questions().utterances().size() << std::endl;
  *mOutput << "#   answers: " << optState->answers().utterances().size() << std::endl;
  *mOutput << "#     links: " << optState->links().links().size() << std::endl;
  
  if (variables()[oVerbosity].as<int>() > 0) {
    *mOutput << "questions fields: "<< std::endl;
    printFieldInfo(*mOutput, optState->questions().utterances(), variables());
    *mOutput << "answer fields: "<< std::endl;
    printFieldInfo(*mOutput, optState->answers().utterances(), variables());
  }
  
  return Error::NO_ERROR;
}

std::string Info::description() const
{
  return "statistics about the database";
}

std::string Info::name() const
{
  return "info";
}

