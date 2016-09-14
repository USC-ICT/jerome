//
//  Help.hpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Help_hpp
#define Help_hpp

#include "Command.hpp"

class Help : public Command {
private:
  po::options_description hiddenOptions() const override;
  po::positional_options_description positionalOptions() const override;
  po::command_line_parser optionsParser(const std::vector<std::string>& args) const override;

  std::string name() const override;
  std::string description() const override;
  void manual(std::ostream& os) const override;
  OptionalError run() override;
};

#endif /* Help_hpp */
