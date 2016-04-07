//
//  ModelWriterText.hpp
//
//  Created by Anton Leuski on 9/4/15.
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

#ifndef __jerome_npc_detail_ModelWriterText_hpp__
#define __jerome_npc_detail_ModelWriterText_hpp__

#include <jerome/npc/model.hpp>
#include <jerome/npc/detail/ModelWriter.hpp>

namespace jerome {
  namespace npc {
		namespace detail {

      class ModelWriterText
        : public ModelWriter
      {
      public:
				static void write(std::ostream&, const Utterance& inUtterance);
				static void write(std::ostream&, const State::ranker_model_type& inRankerModel);
        static void write(std::ostream&, const Link& link) noexcept(false);
        static void write(std::ostream&, const Domain& domain) noexcept(false);
        static void write(std::ostream&, const Lattice& lattice) noexcept(false);
        static void write(std::ostream&, const State& state) noexcept(false);
        static void write(std::ostream&, const Collection& collection) noexcept(false);
			};
			
		}

		template <typename T>
		typename std::enable_if<std::is_base_of<ModelObject, T>::value, std::ostream&>::type
		operator << (std::ostream& outs, const T& obj)
    {
			detail::ModelWriterText::write(outs, obj);
			return outs;
    }

	}
}

#endif // defined __jerome_npc_detail_ModelWriterText_hpp__
