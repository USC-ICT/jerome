//
//  RankerFactory.hpp
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

#ifndef __jerome_npc_factories_RankerFactory_hpp__
#define __jerome_npc_factories_RankerFactory_hpp__

#include <jerome/npc/detail/Ranker_impl.hpp>
#include <jerome/type/Factory.hpp>
#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/math/parameters/types.hpp>

namespace jerome {

	namespace npc {
	
		class State;
		
    class RankerFactory
      : public Factory<
        RankerFactory,
				detail::Ranker,
				const Record&,
        const detail::Data&,
        const math::parameters::value_vector&>
    {
    public:
			typedef Factory<
        RankerFactory,
				detail::Ranker,
				const Record&,
        const detail::Data&,
        const math::parameters::value_vector&> parent_type;
		
			static constexpr const char* CLASSIFIER_RECORD = "classifier";
		
      RankerFactory();
			Result<object_type> make(const Record&,
        const detail::Data&,
        const math::parameters::value_vector&);
			
      template <typename BaseRanker>
			void registerProvider();
			
    };

		namespace detail {
      template <typename BaseRanker>
      struct DefaultRankerProvider
        : public RankerFactory::Provider
      {
				static constexpr const char* IDENTIFIER = BaseRanker::IDENTIFIER;
			
        Result<Ranker> provide(const Record& inModel,
                               const Data& inData,
                               const ir::value_vector& inParams) override
        {
					typedef Providable<ClassifierImplementationTemplate<BaseRanker>> Impl;
					auto impl = std::make_shared<Impl>(String(IDENTIFIER), inModel, inData);
					if (!impl) return Error("failed to instantiate ranker");
					auto ranker = Ranker(impl);
					ranker.setValues(inParams);
					return ranker;
        }

      };
		}
		
		template <typename BaseRanker>
		void RankerFactory::registerProvider()
		{
			registerProviderClass<detail::DefaultRankerProvider<BaseRanker>>();
		}

    struct UtteranceCLRankerModel
    {
      static constexpr const char* IDENTIFIER = "jerome.ranker.cl";
      static constexpr const char* ANSWER_WEIGHTING_KEY = "answer.weighting";
      static constexpr const char* QUESTION_WEIGHTING_KEY = "question.weighting";
      static constexpr const char* ANSWER_ANALYZER_KEY = "answer.analyzer";
      static constexpr const char* QUESTION_ANALYZER_KEY = "question.analyzer";
      static constexpr const char* USE_LOOKUP_TABLE_KEY = "use.lookup.table";
    };
   
	}
}

#endif // defined __jerome_npc_factories_RankerFactory_hpp__
