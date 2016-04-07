//
//  Ranker.cpp
//
//  Created by Anton Leuski on 7/31/15.
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


#include <jerome/ir/training/Data.hpp>
#include <jerome/ir/rm/AnyWeighting.hpp>

#include <jerome/npc/parsing.hpp>
#include <jerome/npc/detail/Ranker_impl.hpp>


#include "Ranker.hpp"

namespace jerome {
  namespace npc {
    namespace detail {

      Data dataFromState(const State& state,
                         const Collection::utterance_index_type& utteranceIndex)
      {
        FieldValueExpander    expandFieldValues(utteranceIndex);
        Data      data(
          state.questions().utterances(),
          state.answers().utterances() | boost::adaptors::transformed(
            expandFieldValues),
          state.links().links());
        return data;
      }

		}
  }
}