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

static const char* oInputFile   = "input";
static const char* oOutputFile   = "output";

po::options_description Info::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  options.add_options()
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
static void printFieldInfo(std::ostream& os, const T& range)
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
  for(const auto& key : fieldKeys) {
    const auto& field = fields[key];
    os << tab << std::setw(12) << std::left << key << " " << field.size() << std::endl;
    if (field.size() > 10) continue;
    List<pair_type> values(field.begin(), field.end());
    std::sort(values.begin(), values.end(),
              [](const pair_type& a, const pair_type& b)
              { return b.second < a.second; });
    for(const auto& v : values) {
      os << tab << tab << std::setw(12) << v.first << ": " << v.second << std::endl;
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
  
  *mOutput << "questions fields: "<< std::endl;
  printFieldInfo(*mOutput, optState->questions().utterances());
  *mOutput << "answer fields: "<< std::endl;
  printFieldInfo(*mOutput, optState->answers().utterances());
  
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

