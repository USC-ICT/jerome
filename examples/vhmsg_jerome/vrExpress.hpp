//
//  vrExpress.hpp
//
//  Created by Anton Leuski on 9/30/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef vrExpress_hpp
#define vrExpress_hpp

#include <jerome/types.hpp>

namespace ict {
  namespace vhmsg {

    using namespace jerome;

    class vrExpress
    {
    public:
      static String compose(const String& inSpeakerID,
                            const String& inText,
                            const String& inReferenceID,
                            const String& inInterruptID);
    };

  }
}
#endif /* vrExpress_hpp */