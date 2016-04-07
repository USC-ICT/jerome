//
//  analysis.hpp
//
//  Created by Anton Leuski on 3/13/12.
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

#ifndef __jerome_ir_analysis_hpp__
#define __jerome_ir_analysis_hpp__

#include <jerome/type/Record.hpp>
#include <jerome/ir/collection.hpp>

namespace jerome {
  namespace ir {
    
    namespace detail {
      template <class D, class Index>
      class AnalyzerImplementation {
        String	mName;
      public:
        typedef	Index		result_type;
        typedef const D&	argument_type;
        AnalyzerImplementation(const String& inName = "")
        : mName(inName) {}
        virtual ~AnalyzerImplementation() {}
        const String& name() const { return this->mName; }
        virtual void parse(argument_type inObject, result_type& ioIndex) const {}
				virtual Record model() const { return Record(); }
			protected:
				void setName(const String& inName) { mName = inName; }
      };
    }
    
    template <class D, class Index>
    class Analyzer : public ReferenceClassInterface<detail::AnalyzerImplementation<D,Index>>
    {
    public:
      typedef	Index		result_type;
      typedef const D&	argument_type;
      typedef detail::AnalyzerImplementation<D,Index> implementation_type;
      typedef ReferenceClassInterface<implementation_type> parent_type;
      
      using parent_type::parent_type;
            
      const String& name() const { return this->implementation().name(); }
      
      void parse(argument_type inObject, result_type& ioIndex) const {
        this->implementation().parse(inObject, ioIndex); }
      
      result_type operator () (argument_type inObject) const {
        result_type	indexedQuery;
        this->parse(inObject, indexedQuery);
        indexedQuery.optimize();
        return indexedQuery;
      }
      
      template <typename Impl, typename ...Args>
      static Analyzer make(Args&& ...args)
      {
        return Analyzer(make_impl<Impl>(std::forward<Args>(args)...));
      }

			Record model() const
			{
				return this->implementation().model();
			}
			
		protected:
			template <typename Impl, typename ...Args>
			static shared_ptr<Impl> make_impl(Args&& ...args)
			{
				return std::make_shared<Impl>(std::forward<Args>(args)...);
			}
			
    };
    
  }
}

#endif // defined __jerome_ir_analysis_hpp__
