//
//  Split.hpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Split_hpp
#define Split_hpp

#include "ClassifierCommand.hpp"

class Split : public ClassifierCommand {
  typedef ClassifierCommand parent_type;
  po::options_description options(po::options_description options) const override;
  std::string name() const override;
  std::string description() const override;
  OptionalError setup() override;
  OptionalError teardown() override;
  OptionalError run1Classifier(const std::string& inName) override;
};

#endif /* Split_hpp */
