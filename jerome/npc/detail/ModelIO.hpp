//
//  ModelIO.hpp
//
//  Created by Anton Leuski on 8/22/15.
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
