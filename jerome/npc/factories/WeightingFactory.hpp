//
//  WeightingFactory.hpp
//
//  Created by Anton Leuski on 9/1/15.
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
