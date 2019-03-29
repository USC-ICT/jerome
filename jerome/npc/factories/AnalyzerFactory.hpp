//
//  AnalyzerFactory.hpp
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

#ifndef __jerome_npc_factories_AnalyzerFactory_hpp__
#define __jerome_npc_factories_AnalyzerFactory_hpp__

#include <jerome/ir/analysis.hpp>
#include <jerome/npc/factories/RecordFactory.hpp>

namespace jerome {

	class Record;
	
	namespace ir {
		template <typename, typename>
		struct Analyzer;
		struct HeapIndex;		
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
