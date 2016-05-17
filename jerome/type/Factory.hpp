//
//  Factory.hpp
//
//  Created by Anton Leuski on 8/23/15.
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

#ifndef __jerome_type_Factory_hpp__
#define __jerome_type_Factory_hpp__

#include <jerome/types.hpp>
#include <jerome/type/Record.hpp>

namespace jerome {

	namespace detail {
		struct FactoryConst {
			static constexpr const char* PROVIDER_IDENTIFIER_KEY = "provider.identifier";
		};
	}

	template <typename Base>
	struct Providable : public Base, detail::FactoryConst {
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
			if (mProviderID)
				m.emplace_front(PROVIDER_IDENTIFIER_KEY, *mProviderID);
			return m;
		}
		
	private:
		OptionalString mProviderID;
	};
	
  template <typename Derived, typename Object, typename ... Args>
	class Factory : detail::FactoryConst
  {
  public:
    struct Provider
    {
      virtual ~Provider()
      {
      }

      virtual Result<Object> operator () (Args ... args) = 0;
    };
		
		typedef Object object_type;
    typedef Provider provider_type;
    typedef std::unique_ptr<provider_type> provider_pointer;

		Result<object_type> make(const optional<String>& identifier, Args&& ... args)
		{
			if (identifier) {
				auto provider = providerWithID(*identifier);
				return provider ? provider.value()(std::forward<Args>(args) ...) : provider.error();
			}
			
			if (defaultProviderID()) {
				auto provider = providerWithID(*defaultProviderID());
				return provider ? provider.value()(std::forward<Args>(args) ...) : provider.error();
			}
			
			return Error("no provider id specified and no default provider defined");
		}

		Result<object_type> make(const Record& record, Args&& ... args)
		{
			return make(record.at<String>(PROVIDER_IDENTIFIER_KEY),
									std::forward<Args>(args) ...);
		}

    Result<provider_type&> providerWithID(const String& identifier)
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
		void registerProviderClass()
		{
			registerProviderForID(provider_pointer(new T), T::IDENTIFIER);
		}

    static Derived& sharedInstance()
    {
      // STATIC
      static auto theFactory = new Derived;
      return *theFactory;
    }

		OptionalString defaultProviderID() const { return mDefaultProviderID; }

  private:
    typedef StringMap<provider_pointer>  providers_type;
    providers_type  mProviders;
		OptionalString mDefaultProviderID;

  };

}
#endif // defined __jerome_type_Factory_hpp__
