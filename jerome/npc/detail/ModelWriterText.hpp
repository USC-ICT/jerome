//
//  ModelWriterText.hpp
//
//  Created by Anton Leuski on 9/4/15.
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
