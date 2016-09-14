//
//  ClassifierCommand.hpp
//  jerome
//
//  Created by Anton Leuski on 9/12/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef ClassifierCommand_hpp
#define ClassifierCommand_hpp

#include "PlatformCommand.hpp"

class ClassifierCommand : public PlatformCommand {
protected:
  typedef PlatformCommand parent_type;
  po::options_description options(po::options_description options) const override;
private:
  std::vector<std::string> classifierNames() const;
  OptionalError run() override;
  virtual OptionalError setup();
  virtual OptionalError teardown();
  virtual OptionalError run1Classifier(const std::string& inName);
};

#endif /* ClassifierCommand_hpp */
