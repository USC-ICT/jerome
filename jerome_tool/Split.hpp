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
  void run(const po::variables_map& vm) override;
private:
};

#endif /* Split_hpp */
