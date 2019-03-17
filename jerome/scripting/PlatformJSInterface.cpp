//
//  PlatformJSInterface.cpp
//
//  Created by Anton Leuski on 8/17/15.
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

#include <jerome/npc/engine.hpp>
#include <jerome/scripting/npc_model.hpp>
//#include <jerome/scripting/javascript/conversion.hpp>
#include "PlatformJSInterface.hpp"

using namespace jerome::npc;

namespace jerome {
  namespace scripting {

    std::vector<Utterance>  PlatformJSInterface::search(
      const String& theNameValue,
      js::Value theQueryValue)
    {
      if (theQueryValue.isObject()) {
        jerome::npc::cpp::ObjectFactory   of;
        Utterance query(of.newUtterance());
        for (const String& p : theQueryValue.propertyNames()) {
          js::Value v = theQueryValue[p];
          if (v.isUndefined() || v.isNull()) continue;
          query.put(p, (String)v);
        }
        return implementation().search(theNameValue, query);
      } else {
        Utterance query(Engine::queryForString((String)theQueryValue));
        return implementation().search(theNameValue, query);
      }
    };

    std::vector<Utterance>  PlatformJSInterface::answers(
      const String& inName)
    {

      if (js::Context::currentArgumentCount() == 1) {
        return implementation().answers(inName);
      }

      return implementation().answers();
    }

    optional<Utterance> PlatformJSInterface::utteranceWithID(
      const String& inUtteranceID)
    {
      return implementation().utteranceWithID(inUtteranceID);
    }

    class EngineEventImpl
      : public EngineEvent
    {
    public:
      EngineEventImpl(const String& inName,
        const OptionalString& inType,
        const OptionalString& inTarget,
        const OptionalString& inOrigin,
        const StringStringMap& inData)
        : mName(inName)
        , mType(inType)
        , mTarget(inTarget)
        , mOrigin(inOrigin)
        , mData(inData)
      {
      }

      String name() const override
      {
        return mName;
      }

      OptionalString type() const override
      {
        return mType;
      }

      OptionalString target() const override
      {
        return mTarget;
      }

      OptionalString origin() const override
      {
        return mOrigin;
      }

      StringStringMap data() const override
      {
        return mData;
      }

    private:
      String  mName;
      OptionalString mType;
      OptionalString mTarget;
      OptionalString mOrigin;
      StringStringMap mData;
    };

    void PlatformJSInterface::receiveEvent(const String& inName,
                                                 const OptionalString& inType,
                                                 const OptionalString& inTarget,
                                                 const OptionalString& inOrigin,
                                                 const StringStringMap& inData)
    {
      implementation().handleEngineEvent(EngineEventImpl(inName, inType,
          inTarget, inOrigin, inData));
    }

  }
}

namespace jerome {
  namespace javascript {

#pragma mark - Engine class definition traits

    ClassTraits<jerome::scripting::PlatformJSInterface>::ClassTraits()
    {
      using namespace jerome::scripting;
      AL_JS_EXPORT_MEMBER_METHOD(search);
      AL_JS_EXPORT_MEMBER_METHOD(answers);
      AL_JS_EXPORT_MEMBER_METHOD(utteranceWithID);
      AL_JS_EXPORT_MEMBER_METHOD(receiveEvent);
    }

  }
}
