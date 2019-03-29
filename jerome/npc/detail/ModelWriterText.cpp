//
//  ModelWriterText.cpp
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

#include "ModelWriterText.hpp"

namespace jerome {
  namespace npc {
		namespace detail {

			template <typename Seq>
			static void writeSeq(std::ostream& os, const Seq& seq)
			{
        for (const auto& v : seq) {
          os << "\n\t";
					ModelWriterText::write(os, v);
        }
			}
			
			template <typename T>
			static void writeModelObject(std::ostream& os, const T& inObject)
			{
				os << T::NAME;
			}
			
			template <typename T>
			static void writeNamedModelObject(std::ostream& os, const T& inObject)
			{
				writeModelObject(os, inObject);
				os << ", name = " << inObject.name();
			}

			void ModelWriterText::write(std::ostream& os, const Utterance& inObject)
			{
				writeModelObject(os, inObject);
        for (const auto& f : inObject.fieldNames()) {
          os << ", " << f << " = " << inObject.get(f, "N/A");
        }
			}
			
			void ModelWriterText::write(std::ostream& os, const State::ranker_model_type& inObject)
			{
			}
			
			void ModelWriterText::write(std::ostream& os, const Link& inObject)
			{
				writeModelObject(os, inObject);
				os << ", value = " << (int)inObject.value();
			}
			
			void ModelWriterText::write(std::ostream& os, const Domain& inObject)
			{
				writeNamedModelObject(os, inObject);
        writeSeq(os, inObject.utterances());
			}
			
			void ModelWriterText::write(std::ostream& os, const Lattice& inObject)
			{
				writeNamedModelObject(os, inObject);
        writeSeq(os, inObject.links());
			}
			
			void ModelWriterText::write(std::ostream& os, const State& inObject)
			{
				writeNamedModelObject(os, inObject);
//        os << ", param = " << inObject.parameters();
        //		foreach(const utterances_type::value_type& v, utterances()) {
        //			os << "\n\t" << v;
        //		}
			}
			
			void ModelWriterText::write(std::ostream& os, const Collection& inObject)
			{
				writeModelObject(os, inObject);
        os << "\nutterances";
        writeSeq(os, inObject.utterances());
        os << "\nlinks";
        writeSeq(os, inObject.links());
        os << "\ndomains";
        writeSeq(os, inObject.domains());
        os << "\nlattices";
        writeSeq(os, inObject.lattices());
        os << "\nstates";
        writeSeq(os, inObject.states());
			}
			
		}
	}
}
