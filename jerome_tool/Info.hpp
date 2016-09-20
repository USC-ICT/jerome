//
//  Info.hpp
//  jerome
//
//  Created by Anton Leuski on 9/20/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Info_hpp
#define Info_hpp

#include "ClassifierCommand.hpp"

class Info : public ClassifierCommand {
  typedef ClassifierCommand parent_type;
  po::options_description options(po::options_description options) const override;
  std::string name() const override;
  std::string description() const override;
  OptionalError setup() override;
  OptionalError run1Classifier(const std::string& inName) override;
  ostream_ptr mOutput;
};

#endif /* Info_hpp */
