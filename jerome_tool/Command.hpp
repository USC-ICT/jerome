//
//  Command.hpp
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

#ifndef Command_hpp
#define Command_hpp

#include <string>
#include <jerome/types.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#include <boost/program_options.hpp>
#pragma clang diagnostic pop

namespace po = boost::program_options;

using namespace jerome;

typedef jerome::shared_ptr<std::ostream> ostream_ptr;
typedef jerome::shared_ptr<std::istream> istream_ptr;

class Commander;

class Command : public std::enable_shared_from_this<Command> {

public:
  virtual ~Command();
  
  static ostream_ptr nullOStream();
  static ostream_ptr ostreamWithName(const po::variable_value& name);
  static ostream_ptr ostreamWithName(const std::string& name);
  static istream_ptr nullIStream();
  static istream_ptr istreamWithName(const po::variable_value& name);
  static istream_ptr istreamWithName(const std::string& name);
  
  void printManual(std::ostream& os) const {
    manual(os);
  }
  
  OptionalError parseArguments(const std::vector<std::string>& args,
                               po::variables_map vm)
  {
    mVariables = vm;
    return parseAndRun(args);
  }
  
protected:
  Command() = default; // cannot create base class
  
  /**
   returns the visible opton descriptions. Sublcasses must call 
   superclass implementation.

   @param ::options_description reuse the passed in value

   @return visible opton descriptions
   */
  virtual po::options_description options(po::options_description options =
                                          po::options_description()) const
  { return options; }
  
  /**
   returns the hidden options. Sublcasses must call superclass implementation.

   @return hidden option descriptions
   */
  virtual po::options_description hiddenOptions() const
  { return po::options_description(); }

  /**
   returns positional options. Sublcasses must call superclass implementation.

   @return psitional option descriptions
   */
  virtual po::positional_options_description positionalOptions() const
  { return po::positional_options_description(); }
   
  /**
   option values collected from the command line

   @return option values
   */
  const po::variables_map& variables() const
  { return mVariables; }

  void storeParsedResults(const po::parsed_options& parsed);
  
private:
  po::variables_map mVariables;
  
  /**
   returns command name option. Subclasses must implement.

   @return command name
   */
  virtual std::string name() const = 0;

  /**
   returns short command summary. Subclasses must implement.
   
   @return command name
   */
  virtual std::string description() const = 0;

  /**
   Prints details command manual into the given stream.

   @param os output stream
   */
  virtual void manual(std::ostream& os) const;
  
  /**
   Executes the command. Assume that command line has been parsed and all options
   are collected in variables().

   @return an error if any
   */
  virtual OptionalError run() = 0;

  /**
   Parses command arguments and executes the command.

   @param args command arguments

   @return an error if any.
   */
  virtual OptionalError parseAndRun(const std::vector<std::string>& args);
  
  friend class Commander;
};

class Commander {
public:
  
  static Commander& shared();
  
  template <class T>
  void registerClass() {
    command<T>();
  }
  
  template <class T>
  T& command() {
    std::shared_ptr<T>        newT = std::make_shared<T>();
    std::shared_ptr<Command>  newCommand = std::dynamic_pointer_cast<Command>(newT);
    auto ptr = std::dynamic_pointer_cast<T>(command_ptr(newCommand->name()));
    if (!ptr) {
      commands().push_back(newT);
      return *newT;
    } else {
      return *ptr;
    }
  }
  
  Command& command(const std::string& inName);
  bool hasCommandWithName(const std::string& inName);
  OptionalError parseArguments(int argc, const char * argv[]);
  std::string executable();
  std::string executablePath();
  void usage(std::ostream& out);
  void printCommandList(std::ostream& out);
  
private:
  std::shared_ptr<Command> command_ptr(const std::string& inName);
  typedef std::vector<std::shared_ptr<Command>> Commands;
  Commands mCommands;
  Commands& commands() { return mCommands; }
};

#endif /* Command_hpp */
