//
//  ModelWriterText.cpp
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
