//
//  model_impl.cpp
//
//  Created by Anton Leuski on 8/21/15.
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