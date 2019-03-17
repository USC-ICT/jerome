//
//  model_impl.cpp
//
//  Created by Anton Leuski on 8/21/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#include "model_impl.hpp"


namespace jerome {
  namespace npc {
    namespace detail {
      
      ModelObjectImpl::~ModelObjectImpl()
      {}
      
      UtteranceIndexImpl::~UtteranceIndexImpl()
      {
      }
            
			UtteranceEnvelope::UtteranceEnvelope(const pointer_type& inSource)
				: parent_type(inSource)
			{
			}

			UtteranceEnvelope::UtteranceEnvelope(const Utterance& inSource)
				: parent_type(inSource)
			{
			}

			std::size_t UtteranceEnvelope::hash() const
			{
				return source().hash();
			}

			bool UtteranceEnvelope::equal_to(const ModelObjectImpl& x) const
			{
				return source().equal_to(x);
			}

			String UtteranceEnvelope::get(const String& key) const
			{
				return source().get(key);
			}

			String UtteranceEnvelope::get(const String& key, const String& inDefault) const
			{
				return source().get(key, inDefault);
			}

			bool UtteranceEnvelope::has(const String& key) const
			{
				return source().has(key);
			}

			void UtteranceEnvelope::put(const String& key, const String& value)
			{
				source().put(key, value);
			}

			void UtteranceEnvelope::remove(const String& key)
			{
				source().remove(key);
			}

			const UtteranceEnvelope::field_names_type& UtteranceEnvelope::fieldNames() const
			{
				return source().fieldNames();
			}

			double UtteranceEnvelope::count() const
			{
				return source().count();
			}

			void UtteranceEnvelope::setCount(double inCount)
			{
				source().setCount(inCount);
			}
    }
  }
}
