//
//  model_cpp.hpp
//
//  Created by Anton Leuski on 9/15/12.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_npc_model_cpp_hpp__
#define __jerome_npc_model_cpp_hpp__

#include <jerome/npc/model.hpp>

namespace jerome {
  namespace npc {
    namespace cpp {
      struct ObjectFactory
        : jerome::npc::ObjectFactory
      {
        jerome::npc::Utterance    newUtterance() const override;
        jerome::npc::Link       newLink() const override;
        jerome::npc::Lattice      newLattice() const override;
        jerome::npc::Domain     newDomain() const override;
        jerome::npc::State      newState(const Domain& inQuestions,
          const Domain& inAnswers,
          const Lattice& inLinks) const override;
        jerome::npc::Collection   newCollection() const override;

      };
    }
  }
}

#endif // defined __jerome_npc_model_cpp_hpp__
