//
//  PlatformCommand.hpp
//  jerome
//
//  Created by Anton Leuski on 9/12/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
  
private:
  Platform mPlatform;
};

#endif /* PlatformCommand_hpp */
