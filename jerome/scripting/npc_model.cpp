//
//  npc_model.cpp
//  jerome
//
//  Created by Anton Leuski on 5/10/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#include "npc_model.hpp"

namespace jerome {
  namespace javascript {

    bool  ClassTraits<jerome::npc::Utterance>::hasProperty(Value& object,
                                                           const String& propertyName)
    {
      const utterance_type& utt(representedObject(object));
      return utt.has(propertyName)
      ? true
      : parent_type::hasProperty(object, propertyName);
    }
    
    Value ClassTraits<jerome::npc::Utterance>::getProperty(Value& object,
                                                           const String& propertyName)
    {
      const utterance_type& utt(representedObject(object));
      return utt.has(propertyName)
      ? object.context().newValue(utt.get(propertyName))
      : parent_type::getProperty(object, propertyName);
    }
    
    bool  ClassTraits<jerome::npc::Utterance>::setProperty(Value& object,
                      const String& propertyName,
                      const Value& value)
    {
      utterance_type& utt(representedObject(object));
      utt.put(propertyName, (String)value);
      return true;
    }
    
    bool  ClassTraits<jerome::npc::Utterance>::deleteProperty(Value& object,
                                                              const String& propertyName)
    {
      utterance_type& utt(representedObject(object));
      if (utt.has(propertyName)) {
        utt.remove(propertyName);
        return true;
      }
      return parent_type::deleteProperty(object, propertyName);
    }
    
    std::vector<std::string>  ClassTraits<jerome::npc::Utterance>::getPropertyNames(const Value& object)
    {
      std::vector<std::string>  props = parent_type::getPropertyNames(object);
      const utterance_type& utt(representedObject(object));
      for (const auto& p : utt.fieldNames()) {
        props.push_back(p);
      }
      return props;
    }
    
    ClassTraits<jerome::npc::Utterance>::ClassTraits()
    {
      AL_JS_EXPORT_MEMBER_METHOD(has);
      // there are two "get" methods, I have to specify the one I want to expose
      AL_JS_EXPORT_MEMBER_METHOD_OF_TYPE_AS(get,
                                            jerome::String (jerome::npc::Utterance::*)(const jerome::String&) const,
                                            "get");
      // I want expose "put" as "set"
      AL_JS_EXPORT_MEMBER_METHOD_AS(put, "set");
    }

  }
}
