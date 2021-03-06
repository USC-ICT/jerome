//
//  Record.hpp
//
//  Created by Anton Leuski on 9/14/15.
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

#ifndef __jerome_type_Record_hpp__
#define __jerome_type_Record_hpp__

#include <jerome/type/ordered_multimap.hpp>

namespace jerome {
	
	class Record;
  
  struct Bool {
    Bool() : mValue(false) {}
    explicit Bool(bool inValue) : mValue(inValue) {}
    bool value() const { return mValue; }
  private:
    bool mValue;
  };
  
  inline std::ostream& operator << (std::ostream& os, const Bool& value)
  {
    return os << std::boolalpha << value.value();
  }
	
	namespace detail {
		typedef boost::variant<String, double, Bool,
		boost::recursive_wrapper<Record>> Record_Field;
		
		template <typename V>
		struct FieldVisitor : public boost::static_visitor<>
		{
			V& mVisitor;
			const String& mName;
			
			FieldVisitor(const String& inName, V& inVisitor)
			: mVisitor(inVisitor)
			, mName(inName)
			{
			}
			
			template <typename T>
			void operator () (T& value)
			{
				mVisitor(mName, value);
			}
			
			template <typename T>
			void operator () (T& value) const
			{
				mVisitor(mName, value);
			}
			
		};
	}
	
	class Record
	: public ordered_multimap<String, detail::Record_Field>
	{
	private:
		void construct() {}
		
		template <class Arg1, class ...Args>
		void construct(const String& key1, Arg1&& val1, Args&& ... rest)
		{
			emplace(key1, std::forward<Arg1>(val1));
			construct(std::forward<Args>(rest)...);
		}
		
	public:
		typedef detail::Record_Field field_type;
		typedef ordered_multimap<String, field_type> parent_type;
		using parent_type::parent_type;
		
		Record() = default;
		
		//		template <class Arg1, class ...Args>
		//		Record(const String& key1, Arg1&& val1, Args&& ... rest)
		//		{
		//			construct(key1, std::forward<Arg1>(val1), std::forward<Args>(rest)...);
		//		}
		
		template <class ...Args>
		Record(const String& a, Args ... rest)
		{
			construct(a, rest...);
		}
		
    size_t count(const String& inKey) const {
      return parent_type::count(inKey);
    }
    
		template <class T>
		void emplace(const String& inKey, T t)
		{
			parent_type::emplace(inKey, t);
		}
		
		template <class T>
		void emplace_back(const String& inKey, T t)
		{
			parent_type::emplace_back(inKey, t);
		}
		
		template <class T>
		void emplace_front(const String& inKey, T t)
		{
			parent_type::emplace_front(inKey, t);
		}
		
		template <typename T>
		T at(const key_type& inKey, const T& inDefault) const
		{
			auto opt = parent_type::at(inKey);
			if (!opt) return inDefault;
			if (const T* p = boost::get<const T>(&(*opt))) {
				return *p;
			}
			return inDefault;
		}
		
    String at(const key_type& inKey, const char* inDefault) const
    {
      auto opt = this->at<String>(inKey);
      if (opt) return *opt;
      return inDefault;
    }

    template <typename T>
		optional<T> at(const key_type& inKey) const
		{
			auto opt = parent_type::at(inKey);
			const T* p;
			if (opt && (p = boost::get<const T>(&(*opt)))) {
				return *p;
			}
			return optional<T>();
		}
		
		template <typename V>
		void visit(V& visitor)
		{
			for (auto& p : * this) {
				detail::FieldVisitor<V> v(p.first, visitor);
				boost::apply_visitor(v, p.second);
			}
		}
		
		template <typename V>
		void visit(V& visitor) const
		{
			for (auto& p : * this) {
				detail::FieldVisitor<V> v(p.first, visitor);
				boost::apply_visitor(v, p.second);
			}
		}
		
    template <typename V>
    void visit(const V& visitor)
    {
      for (auto& p : * this) {
        detail::FieldVisitor<V> v(p.first, visitor);
        boost::apply_visitor(v, p.second);
      }
    }
    
    template <typename V>
    void visit(const V& visitor) const
    {
      for (auto& p : * this) {
        detail::FieldVisitor<V> v(p.first, visitor);
        boost::apply_visitor(v, p.second);
      }
    }
    
		struct Visitor {
			
			template <typename T>
			void operator () (const String& name, T& value)
			{}
			template <typename T>
			void operator () (const String& name, T& value) const
			{}
			
		};
    
  private:
    
    template <typename T>
    struct ValueCollector
    : public Visitor
    {
      using Visitor::operator();
      List<T>  values;
      
      void operator () (const String& inKey, const T& inValue)
      {
        values.push_back(inValue);
      }
    };

  public:
		
    template <typename T>
    List<T> allValuesOfType() const {
      ValueCollector<T> collector;
      this->visit(collector);
      return collector.values;
    }
    
	};
	
	//	namespace detail {
	//		class Record_Field : public Record::Field {
	//			typedef Record::Field parent_type;
	//			using parent_type::parent_type;
	//		};
	//	}
	
}

#endif // defined __jerome_type_Record_hpp__
