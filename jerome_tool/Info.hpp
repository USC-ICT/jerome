//
//  Info.hpp
//  jerome
//
//  Created by Anton Leuski on 9/20/16.
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
