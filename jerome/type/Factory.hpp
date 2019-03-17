//
//  Factory.hpp
//
//  Created by Anton Leuski on 8/23/15.
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

#ifndef __jerome_type_Factory_hpp__
#define __jerome_type_Factory_hpp__

#include <jerome/types.hpp>
#include <jerome/type/Record.hpp>
#include <jerome/type/algorithm.hpp>

namespace jerome {

	namespace detail {
		struct FactoryConst {
			static constexpr const char* PROVIDER_KEY = "provider";
		};
	}

	template <typename Base>
	struct Providable : public Base, public detail::FactoryConst {
		typedef Base parent_type;
		
		Providable() = default;
		
		template <typename ...As>
		Providable(const String& inProviderID, As&& ... args)
		: parent_type(std::forward<As>(args)...)
		, mProviderID(inProviderID)
		{}
		
		Record model() const
		{
			Record m = parent_type::model();
      if (!m.at<String>(PROVIDER_KEY) && mProviderID)
				m.emplace_front(PROVIDER_KEY, *mProviderID);
			return m;
		}
		
	private:
		OptionalString mProviderID;
	};
	
  template <typename Derived, typename Object, typename ... Args>
	class Factory : public detail::FactoryConst
  {
  public:
    struct Provider
    {
      virtual ~Provider()
      {
      }

      virtual Result<Object> provide(Args ... args) = 0;
    };
		
		typedef Object object_type;
    typedef Provider provider_type;
    typedef std::unique_ptr<provider_type>  provider_pointer;
    typedef StringMap<Record>               models_type;

		Result<object_type> make(const optional<String>& identifier, Args&& ... args)
		{
			if (identifier) {
        return make(*identifier, std::forward<Args>(args) ...);
			}
			
			if (defaultProviderID()) {
        return make(*defaultProviderID(), std::forward<Args>(args) ...);
			}
			
			return Error("no provider id specified and no default provider defined");
		}

    Result<object_type> make(const String& identifier, Args&& ... args)
    {
      auto provider = providerWithID(identifier);
      return provider
      ? provider.value().provide(std::forward<Args>(args) ...)
      : provider.error();
    }

    Result<provider_type&> providerWithID(const String& identifier) const
    {
      auto p = mProviders.find(identifier);
      if (p == mProviders.end())
        return Error("No provider with identifier " + identifier);
      else
        return *p->second;
    }

    void registerProviderForID(provider_pointer&& ptr, const String& identifier)
    {
      mProviders.emplace(identifier, std::move(ptr));
			if (!mDefaultProviderID) mDefaultProviderID = identifier;
    }

    template <typename T>
    void registerProviderClassForID(const String& id)
    {
      registerProviderForID(provider_pointer(new T), id);
    }
    
    template <typename T>
    void registerProviderClass()
    {
      registerProviderClassForID<T>(T::IDENTIFIER);
    }
    
    static Derived& sharedInstance()
    {
      // STATIC
      static auto theFactory = new Derived;
      return *theFactory;
    }

		OptionalString defaultProviderID() const { return mDefaultProviderID; }

    List<String> providerIDs() const {
      return keys(mProviders);
    }
    
    const models_type& predefinedModels() const
    {
      return mPredefinedModels;
    }
    
    optional<Record> modelAt(const String& inKey) const
    {
      auto x = predefinedModels().find(inKey);
      if (x != predefinedModels().end()) {
        return x->second;
      }
      return optional<Record>();
    }
    
    void registerModel(String inKey, Record inModel)
    {
      mPredefinedModels.emplace(std::make_pair(std::move(inKey),
                                               std::move(inModel)));
    }
    
    String defaultModelKey() const
    {
      return mDefaultModelKey;
    }
    
    Record defaultModel() const
    {
      auto x = predefinedModels().find(mDefaultModelKey);
      if (x != predefinedModels().end()) {
        return x->second;
      }
      x = predefinedModels().begin();
      if (x != predefinedModels().end()) {
        return x->second;
      }
      return Record();
    }

    void setDefaultModelKey(const String& inKey)
    {
      mDefaultModelKey = inKey;
    }
    
  private:
    typedef StringMap<provider_pointer> providers_type;
    providers_type  mProviders;
		OptionalString  mDefaultProviderID;
    String          mDefaultModelKey;
    models_type     mPredefinedModels;

    
  };

}
#endif // defined __jerome_type_Factory_hpp__
