//
//  WeightingFactory.hpp
//
//  Created by Anton Leuski on 9/1/15.
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

#ifndef __jerome_npc_factories_WeightingFactory_hpp__
#define __jerome_npc_factories_WeightingFactory_hpp__

#include <jerome/npc/factories/RecordFactory.hpp>
#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/ir/rm/AnyWeighting.hpp>

namespace jerome {

  class Record;

  namespace npc {

    namespace detail {

			template <template <typename,typename> class Any>
			class ProvidableWeighting
        : public Providable<Any<ProvidableWeighting<Any>, ir::HeapIndex>>
			{
				typedef Providable<Any<ProvidableWeighting<Any>,
          ir::HeapIndex>> parent_type;
				
			public:
				using parent_type::parent_type;
				
				template <template <typename ...> class W, typename ... Args>
				static ProvidableWeighting make(const String& inProviderID,
                                        const String& inName, Args&& ... args)
				{
					return ProvidableWeighting
            (inProviderID, parent_type::template make_impl<W>
              (inName, std::forward<Args>(args) ...));
				}
			};

			typedef ProvidableWeighting<
        jerome::ir::rm::AnyDocumentWeighting> AnswerWeighting;
			typedef ProvidableWeighting<
        jerome::ir::rm::AnyQueryWeighting> QuestionWeighting;

    }

    class QuestionWeightingFactory
      : public RecordFactory<
        QuestionWeightingFactory,
        detail::QuestionWeighting>
    {
    public:
      typedef RecordFactory<
        QuestionWeightingFactory,
        detail::QuestionWeighting> parent_type;

      QuestionWeightingFactory();

      using parent_type::make;
    };

    class AnswerWeightingFactory
      : public RecordFactory<
        AnswerWeightingFactory,
        detail::AnswerWeighting>
    {
    public:
      typedef RecordFactory<
        AnswerWeightingFactory,
        detail::AnswerWeighting> parent_type;

      AnswerWeightingFactory();

      using parent_type::make;
   };
		
  }
}

#endif // defined __jerome_npc_factories_WeightingFactory_hpp__
