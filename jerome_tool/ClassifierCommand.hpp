//
//  ClassifierCommand.hpp
//  jerome
//
//  Created by Anton Leuski on 9/12/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
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
