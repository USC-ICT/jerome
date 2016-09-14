//
//  Command.hpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Command_hpp
#define Command_hpp

#include <string>
#include <jerome/types.hpp>
#include <boost/program_options.hpp>

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
  
  OptionalError parseArguments(const std::vector<std::string> args,
                               po::variables_map& vm)
  {
    mVariables = vm;
    return parseAndRun(args);
  }
  
protected:
  Command() = default;
  
  virtual po::options_description options(po::options_description options =
                                          po::options_description()) const
  { return options; }

  virtual po::options_description hiddenOptions() const
  { return po::options_description(); }

  virtual po::positional_options_description positionalOptions() const
  { return po::positional_options_description(); }
  
  virtual po::command_line_parser optionsParser(const std::vector<std::string>& args) const;
  
  const po::variables_map& variables() const
  { return mVariables; }

private:
  po::variables_map mVariables;
  
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual void manual(std::ostream& os) const;
  virtual OptionalError run() = 0;
  virtual OptionalError parseAndRun(const std::vector<std::string> args);
  
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
