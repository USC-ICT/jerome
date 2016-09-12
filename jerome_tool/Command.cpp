//
//  Command.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>

#include "Command.hpp"

namespace fs = boost::filesystem;

Command::~Command()
{
}

Command& Command::command(const std::string& inName)
{
  auto ptr = command_ptr(inName);
  if (!ptr) {
    throw std::invalid_argument("unknown command name \"" + inName + "\"");
  } else {
    return *ptr;
  }
}

bool Command::hasCommandWithName(const std::string& inName)
{
  auto ptr = command_ptr(inName);
  return ptr != nullptr;
}

Command::Commands& Command::commands()
{
  static Commands commands;
  return commands;
}

std::shared_ptr<Command> Command::command_ptr(const std::string& inName)
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

std::string Command::executablePath()
{
  return sExecutable;
}

std::string Command::executable()
{
  fs::path e(sExecutable);
  return e.filename().string();
}

void Command::parseArguments(int argc, const char * argv[])
{
  sExecutable = argv[0];
  
  po::options_description all_options("Allowed options");
  all_options.add_options()
  (oCommand, 		po::value<std::string>(), "command")
  (oCommandArgs, 	po::value<std::vector<std::string>>(), "arguments for command")
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
    throw invalid_argument_help("command name is required");
  }
  
  std::string commandName = vm[oCommand].as<std::string>();
  if (!Command::hasCommandWithName(commandName)) {
    throw invalid_argument_help("unknown command \"" + commandName + "\"");
  }
  
  std::vector<std::string> opts = po::collect_unrecognized(parsed.options,
                                                           po::include_positional);
  opts.erase(opts.begin());
  
  Command::command(commandName).parseAndRun(opts, vm);
}

void Command::parseAndRun(const std::vector<std::string> args, po::variables_map& vm)
{
  po::parsed_options parsed = po::command_line_parser(args)
  .options(mOptions)
  .run();
  
  po::store(parsed, vm);
  po::notify(vm);

  run(vm);
}

void Command::manual(std::ostream& out) const
{
  out << description() << std::endl
  << "usage: " << Command::executable()
  << " " << name() << " [<options>]" << std::endl;
  out << options() << std::endl;
}

void Command::usage(std::ostream& out)
{
  out << "usage: " << Command::executable()
  << " <command> [<args>]"
  << std::endl;
  
  out << "These are available commands:" << std::endl;
  for(auto& c : Command::commands()) {
    out << "   "
    << std::setw(10) << std::left << c->name()
    << " "
    << c->description()
    << std::endl;
  }
}

jerome::shared_ptr<std::ostream> Command::ostreamWithName(const std::string& name)
{
  return name == "-" ? jerome::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})
  : jerome::shared_ptr<std::ostream>(new std::fstream(name));
}

jerome::shared_ptr<std::istream> Command::istreamWithName(const std::string& name)
{
  return name == "-" ? jerome::shared_ptr<std::istream>(&std::cin, [](std::istream*){})
  : jerome::shared_ptr<std::istream>(new std::fstream(name));
}


