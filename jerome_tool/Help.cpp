//
//  Help.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "Help.hpp"

void Help::manual(std::ostream& out) const
{
  out << description() << std::endl
  << "usage: " << Commander::shared().executable()
  << " " << name() << " <command>" << std::endl;
  Commander::shared().printCommandList(out);
}

static const char* oCommand     = "_help-command";
static const char* oCommandArgs = "_help-command-args";

po::options_description Help::hiddenOptions() const
{
  auto opts = Command::hiddenOptions();

  opts.add_options()
  (oCommand, 		po::value<std::string>(), "command")
  (oCommandArgs, 	po::value<std::vector<std::string>>(), "arguments for command");
  
  return opts;
}

po::positional_options_description Help::positionalOptions() const
{
  return Command::positionalOptions()
  .add(oCommand, 1)
  .add(oCommandArgs, -1);
}

po::command_line_parser Help::optionsParser(const std::vector<std::string>& args) const
{
  return Command::optionsParser(args).allow_unregistered();
}

OptionalError Help::run() {

  if (!variables()[oCommand].empty()) {
    Commander::shared().usage(std::cout);
    return Error::NO_ERROR;
  }
  
  std::string commandName = variables()[oCommand].as<std::string>();
  if (!Commander::shared().hasCommandWithName(commandName)) {
    return Error("unknown command \"" + commandName + "\"");
  }

  Commander::shared().command(commandName).printManual(std::cout);
  return Error::NO_ERROR;
}

std::string Help::description() const
{
  return "description of the commands";
}

std::string Help::name() const
{
  return "help";
}
