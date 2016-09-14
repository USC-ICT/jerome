//
//  Evaluate.hpp
//  jerome
//
//  Created by Anton Leuski on 9/9/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Evaluate_hpp
#define Evaluate_hpp

#include "ClassifierCommand.hpp"

class Evaluate : public ClassifierCommand {
  typedef ClassifierCommand parent_type;
  po::options_description options(po::options_description options) const override;
  std::string name() const override;
  std::string description() const override;
  OptionalError setup() override;
  OptionalError teardown() override;
  OptionalError run1Classifier(const std::string& inName) override;
};


#endif /* Evaluate_hpp */
