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

class invalid_argument_help : public std::invalid_argument {
public:
  using std::invalid_argument::invalid_argument;
};

class Command : public std::enable_shared_from_this<Command> {

public:
  virtual ~Command();
  
  template <class T>
  static void registerClass() {
    command<T>();
  }
  
  template <class T>
  static T& command() {
    std::shared_ptr<T>  c = std::make_shared<T>();
    auto ptr = std::dynamic_pointer_cast<T>(command_ptr(c->name()));
    if (!ptr) {
      commands().push_back(c);
      return *c;
    } else {
      return *ptr;
    }
  }
  
  static Command& command(const std::string& inName);
  static bool hasCommandWithName(const std::string& inName);
  static void parseArguments(int argc, const char * argv[]);
  static std::string executable();
  static std::string executablePath();
  static void usage(std::ostream& out);

  static jerome::shared_ptr<std::ostream> ostreamWithName(const std::string& name);
  static jerome::shared_ptr<std::istream> istreamWithName(const std::string& name);
  
  std::string name() const {
    return mName;
  }

  void printManual(std::ostream& os) const {
    manual(os);
  }
  
protected:
  Command(const std::string& inName, const std::string& inOptionsCaption)
  : mName(inName)
  , mOptions(inOptionsCaption)
  {}
  
  po::options_description& options() { return mOptions; }
  const po::options_description& options() const { return mOptions; }
  
private:
  std::string mName;
  po::options_description mOptions;
  
  virtual std::string description() const = 0;
  virtual void manual(std::ostream& os) const;
  virtual void run(const po::variables_map& vm) = 0;
  virtual void parseAndRun(const std::vector<std::string> args, po::variables_map& vm);
  
  static std::shared_ptr<Command> command_ptr(const std::string& inName);
  typedef std::vector<std::shared_ptr<Command>> Commands;
  static Commands& commands();
  
};

#endif /* Command_hpp */
