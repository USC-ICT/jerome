//
//  PlatformJSInterface.cpp
//
//  Created by Anton Leuski on 8/17/15.
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