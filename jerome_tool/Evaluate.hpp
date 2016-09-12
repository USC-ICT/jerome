//
//  Evaluate.hpp
//  jerome
//
//  Created by Anton Leuski on 9/9/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Evaluate_hpp
#define Evaluate_hpp

#include "Command.hpp"

class Evaluate : public Command {
public:
  Evaluate();
private:
  std::string description() const override;
  void run(const po::variables_map& vm) override;
};


#endif /* Evaluate_hpp */
