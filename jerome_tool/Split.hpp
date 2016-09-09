//
//  Split.hpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Split_hpp
#define Split_hpp

#include "Command.hpp"

class Split : public Command {
public:
  Split();
  std::string description() const override;
  void manual(std::ostream& os) const override;
  void run(const std::vector<std::string>& args, po::variables_map& vm) override;
private:
  po::options_description mOptions;
};

#endif /* Split_hpp */
