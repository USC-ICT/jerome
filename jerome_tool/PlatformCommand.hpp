//
//  PlatformCommand.hpp
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

#ifndef PlatformCommand_hpp
#define PlatformCommand_hpp

#include <jerome/npc/npc.hpp>
#include "Command.hpp"

using namespace jerome::npc;

class PlatformCommand : public Command {
public:
  PlatformCommand();
  
protected:
  Platform& platform() { return mPlatform; }
  const Platform& platform() const { return mPlatform; }
  OptionalError loadCollection(const po::variable_value& inName, 
                               const po::variable_value& inFormat);
  OptionalError loadCollection();
  void appendInputOptions(po::options_description& ioOptions) const;
  std::string inputFileName(const po::variables_map& inVM) const;
  
private:
  Platform mPlatform;
};

#endif /* PlatformCommand_hpp */
