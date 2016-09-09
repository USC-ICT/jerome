//
//  Train.hpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Train_hpp
#define Train_hpp

#include "Command.hpp"

class Train : public Command {
public:
  Train();
  std::string description() const override;
  void manual(std::ostream& os) const override;
  void run(const std::vector<std::string>& args, po::variables_map& vm) override;
private:
  po::options_description mOptions;
};


#endif /* Train_hpp */
