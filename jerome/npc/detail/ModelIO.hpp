//
//  ModelIO.hpp
//
//  Created by Anton Leuski on 8/22/15.
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

#ifndef __jerome_npc_detail_ModelIO_hpp__
#define __jerome_npc_detail_ModelIO_hpp__

#include <jerome/xml/record_writer.hpp>

namespace jerome {
  namespace npc {
		constexpr const double MODEL_XML_VERSION = 2.0;
		
		namespace detail {
			namespace tag {

#define JEROME_DEFINE_TAG(TAG) constexpr const char* TAG = #TAG;
			
				JEROME_DEFINE_TAG(database)

				JEROME_DEFINE_TAG(utterance)
				JEROME_DEFINE_TAG(count)
				JEROME_DEFINE_TAG(link)
				JEROME_DEFINE_TAG(domain)
				JEROME_DEFINE_TAG(lattice)
				JEROME_DEFINE_TAG(state)

				JEROME_DEFINE_TAG(utterances)
				JEROME_DEFINE_TAG(links)
				JEROME_DEFINE_TAG(domains)
				JEROME_DEFINE_TAG(lattices)
				JEROME_DEFINE_TAG(states)

				JEROME_DEFINE_TAG(nextID)
				JEROME_DEFINE_TAG(field)
				JEROME_DEFINE_TAG(name)
				JEROME_DEFINE_TAG(version)
				JEROME_DEFINE_TAG(id)
				JEROME_DEFINE_TAG(value)

				JEROME_DEFINE_TAG(questions)
				JEROME_DEFINE_TAG(answers)

				JEROME_DEFINE_TAG(classifier)
				JEROME_DEFINE_TAG(parameters)
				JEROME_DEFINE_TAG(parameter)
				JEROME_DEFINE_TAG(properties)
				JEROME_DEFINE_TAG(property)
				JEROME_DEFINE_TAG(type)

#undef JEROME_DEFINE_TAG

			}
			
			typedef xml::RecordTags	CLASSIFIER_TAGS;
			
		}
	}
}

#endif // defined __jerome_npc_detail_ModelIO_hpp__
