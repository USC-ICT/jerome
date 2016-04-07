//
//  model_cpp.hpp
//
//  Created by Anton Leuski on 9/15/12.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Jerome is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Jerome is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with Jerome.  If not, see <http://www.gnu.org/licenses/>.
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
