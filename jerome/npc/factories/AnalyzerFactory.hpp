//
//  AnalyzerFactory.hpp
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

#ifndef __jerome_npc_factories_AnalyzerFactory_hpp__
#define __jerome_npc_factories_AnalyzerFactory_hpp__

#include <jerome/ir/analysis.hpp>
#include <jerome/npc/factories/RecordFactory.hpp>

namespace jerome {

	class Record;
	
	namespace ir {
		template <typename, typename>
		struct Analyzer;
		class HeapIndex;		
	}

  namespace npc {
	
    typedef jerome::ir::Analyzer<Utterance, jerome::ir::HeapIndex> Analyzer;

    class AnalyzerFactory
      : public Factory<
        AnalyzerFactory,
				Analyzer,
        const jerome::ir::AlphabetPtr&,
        const Record&>
    {
      typedef Factory<
      AnalyzerFactory,
      Analyzer,
      const jerome::ir::AlphabetPtr&,
      const Record&> parent_type;
    public:
      AnalyzerFactory();

      typedef typename parent_type::object_type object_type;

      using parent_type::make;

      Result<object_type> make(const jerome::ir::AlphabetPtr& inDictionary, const Record& inRecord)
      {
        return parent_type::make(inRecord.at<String>(parent_type::PROVIDER_KEY),
                                 inDictionary,
                                 inRecord);
      }

      template <typename AnalyzerImplementation>
			void registerProvider();
    };

		namespace detail {
      template <typename Impl>
      struct DefaultAnalyzerProvider
        : public AnalyzerFactory::Provider
      {
				static constexpr const char* IDENTIFIER = Impl::IDENTIFIER;
				
        Result<AnalyzerFactory::object_type> provide(const jerome::ir::AlphabetPtr& inDictionary, const Record& inRecord) override
        {
					return Analyzer::make<Providable<Impl>>(String(IDENTIFIER), inDictionary, inRecord);
        }
      };
		}

		template <typename AnalyzerImplementation>
		void AnalyzerFactory::registerProvider()
		{
			registerProviderClass<detail::DefaultAnalyzerProvider<AnalyzerImplementation>>();
		}

	}
}

#endif // defined __jerome_npc_factories_AnalyzerFactory_hpp__
