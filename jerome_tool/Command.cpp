//
//  Command.cpp
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
#include <iomanip>
#include <boost/filesystem.hpp>

#include "Command.hpp"

namespace fs = boost::filesystem;

Command& Commander::command(const std::string& inName)
{
  auto ptr = command_ptr(inName);
  if (!ptr) {
    throw std::invalid_argument("unknown command name \"" + inName + "\"");
  } else {
    return *ptr;
  }
}

bool Commander::hasCommandWithName(const std::string& inName)
{
  auto ptr = command_ptr(inName);
  return ptr != nullptr;
}

Commander& Commander::shared()
{
  static Commander commander;
  return commander;
}

std::shared_ptr<Command> Commander::command_ptr(const std::string& inName)
{
  for(auto x : commands()) {
    if (inName == x->name()) {
      return x;
    }
  }
  return std::shared_ptr<Command>();
}

static const char* oCommand     = "command";
static const char* oCommandArgs = "command-args";
static std::string sExecutable;

std::string Commander::executablePath()
{
  return sExecutable;
}

std::string Commander::executable()
{
  fs::path e(sExecutable);
  return e.filename().string();
}

OptionalError Commander::parseArguments(int argc, const char * argv[])
{
  sExecutable = argv[0];
  
  po::options_description all_options("Allowed options");
  all_options.add_options()
  (oCommand, 		po::value<std::string>(), "command")
  (oCommandArgs, 	po::value<std::vector<std::string>>(), "command arguments")
  ;
  
  po::positional_options_description positional_options;
  positional_options
  .add(oCommand, 1)
  .add(oCommandArgs, -1);
  
  po::options_description global_options;
  global_options.add_options()
  ;
  all_options.add(global_options);
  
  po::parsed_options parsed = po::command_line_parser(argc, argv)
  .options(all_options)
  .positional(positional_options)
  .allow_unregistered()
  .run();
  
  po::variables_map vm;
  po::store(parsed, vm);
  po::notify(vm);
  
  if (!vm.count(oCommand)) {
    return Error("command name is required");
  }
  
  std::string commandName = vm[oCommand].as<std::string>();
  if (!hasCommandWithName(commandName)) {
    return Error("unknown command \"" + commandName + "\"");
  }
  
  std::vector<std::string> args = po::collect_unrecognized(parsed.options,
                                                           po::include_positional);
  args.erase(args.begin()); // erase command from command arguments
  
  return command(commandName).parseArguments(args, vm);
}

void Commander::usage(std::ostream& out)
{
  out << "usage: " << executable()
  << " <command> [<args>]"
  << std::endl;
  
  printCommandList(out);
}

void Commander::printCommandList(std::ostream& out)
{
  out << "These are the available commands:" << std::endl;
  for(auto& c : commands()) {
    out << "   "
    << std::setw(10) << std::left << c->name()
    << " "
    << c->description()
    << std::endl;
  }
}

// -----------------------------------------------------------------------------

Command::~Command()
{
}

void Command::storeParsedResults(const po::parsed_options &parsed)
{
  po::store(parsed, mVariables);
  po::notify(mVariables);
}

OptionalError Command::parseAndRun(const std::vector<std::string>& args)
{
  po::options_description all_options;
  all_options
  .add(options())
  .add(hiddenOptions());

  // I must make it a variable because the parser will store a pointer
  // to this object
  auto positional_options = positionalOptions();
  
  // note that I cannot break this instructions into seprate files for
  // customization because both command_line_parser and parsed_options
  // store pointers to options_description. An attempt to copy eithoer one
  // of them will lead to a crash. Blame boost program_option library design.
  po::parsed_options parsed = po::command_line_parser(args)
  .options(all_options)
  .positional(positional_options)
  .run();
  
  storeParsedResults(parsed);

  return run();
}

void Command::manual(std::ostream& out) const
{
  out << description() << std::endl
  << "usage: " << Commander::shared().executable()
  << " " << name() << " [<options>]" << std::endl;
  out << options() << std::endl;
}

class NullBuffer : public std::streambuf
{
public:
  int overflow(int c) { return c; }
};

class NullOStream : public std::ostream {
public:
  NullOStream() : std::ostream(&m_sb) {}
private:
  NullBuffer m_sb;
};

class NullIStream : public std::istream {
public:
  NullIStream() : std::istream(&m_sb) {}
private:
  NullBuffer m_sb;
};

ostream_ptr Command::nullOStream()
{
  return ostream_ptr(new NullOStream);
}

istream_ptr Command::nullIStream()
{
  return istream_ptr(new NullIStream);
}

ostream_ptr Command::ostreamWithName(const po::variable_value& value)
{
  return value.empty()
  ? nullOStream()
  : ostreamWithName(value.as<std::string>());
}

ostream_ptr Command::ostreamWithName(const std::string& name)
{
  return name == "-"
  ? ostream_ptr(&std::cout, [](std::ostream*){})
  : ostream_ptr(new std::ofstream(name));
}

istream_ptr Command::istreamWithName(const std::string& name)
{
  return name == "-"
  ? istream_ptr(&std::cin, [](std::istream*){})
  : istream_ptr(new std::ifstream(name));
}

istream_ptr Command::istreamWithName(const po::variable_value& value)
{
  return value.empty()
  ? nullIStream()
  : istreamWithName(value.as<std::string>());
}


