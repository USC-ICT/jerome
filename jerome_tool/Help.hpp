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
public:
  Help();
  std::string description() const override;
  void manual(std::ostream& os) const override;
  void run(const std::vector<std::string>& args, po::variables_map& vm) override;
};

#endif /* Help_hpp */
