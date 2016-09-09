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

Help::Help()
: Command("help")
{}

void Help::manual(std::ostream& out) const
{
  out << description() << std::endl
  << "usage: " << Command::executable()
  << " " << name() << " <command>" << std::endl;
}

static const char* oCommand     = "help-command";
static const char* oCommandArgs = "help-command-args";

void Help::run(const std::vector<std::string>& args, po::variables_map& vm) {
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
  
  po::parsed_options parsed = po::command_line_parser(args)
  .options(all_options)
  .positional(positional_options)
  .allow_unregistered()
  .run();

  po::store(parsed, vm);
  po::notify(vm);

  if (!vm.count(oCommand)) {
    Command::usage(std::cerr);
    return;
  }
  
  std::string commandName = vm[oCommand].as<std::string>();
  if (!Command::hasCommandWithName(commandName)) {
    std::cerr << "unknown command \"" + commandName + "\"" << std::endl;
    Command::usage(std::cerr);
  }

  Command::command(commandName).manual(std::cout);
}

std::string Help::description() const
{
  return "description of the commands";
}
