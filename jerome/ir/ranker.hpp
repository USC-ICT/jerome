//
//  ranker.hpp
//
//  Created by Anton Leuski on 4/11/12.
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

#ifndef __jerome_ir_ranker_hpp__
#define __jerome_ir_ranker_hpp__

#include <jerome/math/parameters/parameterized.hpp>
#include <jerome/math/scored.hpp>
#include <jerome/ir/collection.hpp>
#include <jerome/ir/analysis.hpp>

namespace jerome {  namespace ir {
	
	using namespace jerome::math;
	using namespace jerome::math::parameters;
	
	
	/** ----------------------------------------------------------------------------
	 *
	 */
	
	template <typename D>
	using RankedList = std::vector<ScoredObject<D>>;
	
	//	template <typename D>
	//	std::ostream& operator << (std::ostream& outs, const typename RankedList<D>& obj)
	//	{
	//		outs << obj.size() << std::endl;
	//		for(std::size_t i = 0, n = obj.size(); i < n; ++i) {
	//			outs << (i+1) << ". " << obj[i].score() << " " << obj[i].object() << std::endl;
	//		}
	//		return outs;
	//	}
	
	
	namespace i {
		
		/** ----------------------------------------------------------------------------
		 *
		 */
		
		template <typename Q, typename D>
		struct Ranker : public std::unary_function<Q, std::vector<RankedList<D>>>
		{
			typedef std::unary_function<Q, std::vector<RankedList<D>>>	parent_type;
			typedef Q											query_type;
			typedef D											document_type;
			typedef RankedList<document_type>	ranked_list_type;
			typedef typename parent_type::argument_type			argument_type;
			typedef typename parent_type::result_type			result_type;
		};
		
#define JEROME_INTERNAL_QUERY_RANKER_TYPES(t) \
		typedef typename t::query_type				query_type; \
		typedef typename t::argument_type			argument_type;

#define JEROME_INTERNAL_RESULT_RANKER_TYPES(t) \
		typedef typename t::document_type			document_type; \
		typedef typename t::ranked_list_type		ranked_list_type; \
		typedef typename t::result_type			result_type;

#define JEROME_INTERNAL_RANKER_TYPES(t) \
		JEROME_INTERNAL_QUERY_RANKER_TYPES(t) \
		JEROME_INTERNAL_RESULT_RANKER_TYPES(t)
		
		template <typename Q, typename D>
		struct RankerImplementation : public Ranker<Q,D> {
		private:
			typedef Ranker<Q,D> _RT;
		public:
			JEROME_INTERNAL_RANKER_TYPES(_RT)

			virtual ~RankerImplementation() {}
			virtual result_type	operator() (const argument_type& query) = 0;
			virtual std::size_t countOfDocuments() const = 0;
			virtual std::size_t countOfQueries() const = 0;
						
			virtual void accept(GetValue& visitor) 	const 	= 0;
			virtual void accept(GetTitle& visitor) 	const 	= 0;
			virtual void accept(GetRange& visitor) 	const 	= 0;
			virtual void accept(SetValue& visitor) 			= 0;
		};
		
#define JEROME_INTERNAL_RANKER_REDIRECT_METHODS(X, Y) \
		result_type		operator() (const argument_type& query) Y { return X(query); } \
		std::size_t		countOfDocuments() const Y 				{ return X.countOfDocuments(); } \
		std::size_t		countOfQueries() const Y 					{ return X.countOfQueries(); } \
		void accept(GetValue& visitor) const Y 			{ X.accept(visitor); } \
		void accept(GetTitle& visitor) const Y 			{ X.accept(visitor); } \
		void accept(GetRange& visitor) const Y 			{ X.accept(visitor); } \
		void accept(SetValue& visitor) Y 					{ X.accept(visitor); }
		
		template <
			typename R,
			class Base = RankerImplementation<typename R::query_type, typename R::document_type>
		>
		struct RankerImplementationTemplate : public Base {
		private:
			typedef Base _RT;
		public:
			JEROME_INTERNAL_RANKER_TYPES(_RT)

			typedef R											ranker_type;
			
			RankerImplementationTemplate() = default;

			RankerImplementationTemplate(const ranker_type& inRanker)
			: mRanker(inRanker)
			{}
			
			RankerImplementationTemplate(ranker_type&& inRanker)
			: mRanker(std::forward<ranker_type>(inRanker))
			{}
			
			JEROME_INTERNAL_RANKER_REDIRECT_METHODS(base_ranker(), override)
			
		private:
			ranker_type	mRanker;

		protected:
			ranker_type&					base_ranker() { return mRanker; }
			const ranker_type&		base_ranker() const { return mRanker; }
		};
		
		/** ----------------------------------------------------------------------------
		 *
		 */
		
	}
	
	// Please do not be confused here: this Ranker class exists to "virtualize"
	// actual ranker implementations. It is not a base class for all ranker classes.
	// Think about it as a virtual interface to ranker. For example, use the
	// class to store different ranker objects in a homogeneous collection.
	
	template <typename Q, typename A, class Impl = i::RankerImplementation<Q,A>>
	struct JEROME_SYMBOL_VISIBLE Ranker
		: public Parameterized<Ranker<Q,A,Impl>, ReferenceClassInterface<Impl>>
		, public i::Ranker<Q,A>
	{
	private:
		typedef Impl implementation_type;
		typedef ReferenceClassInterface<implementation_type> actual_parent_type;
		typedef Parameterized<Ranker<Q,A,Impl>, actual_parent_type> parent_type;
		typedef i::Ranker<Q,A> _RT;
	public:
		JEROME_INTERNAL_RANKER_TYPES(_RT)

    using parent_type::parent_type;

	public:
    explicit Ranker(const shared_ptr<implementation_type>& inData)
    : parent_type(inData)
    {}
    explicit Ranker(shared_ptr<implementation_type>&& inData)
    : parent_type(std::move(inData))
    {}
		
	public:
		JEROME_INTERNAL_RANKER_REDIRECT_METHODS(this->implementation(), )
	};
	
	template <typename Q, typename Rnk, typename Index>
	struct JEROME_SYMBOL_VISIBLE RankerWithAnalyzer : public Rnk {
		typedef Rnk parent_type;
		typedef Analyzer<Q, Index> query_analyzer_type;
		
		JEROME_INTERNAL_RESULT_RANKER_TYPES(Rnk)
		typedef Q			query_type;
		typedef Q			argument_type;
		
		template <class R>
		RankerWithAnalyzer(const R& inRanker, const query_analyzer_type& inAnalyzer)
		: parent_type(inRanker)
		, mAnalyzer(inAnalyzer)
		{}

		template <class R>
		RankerWithAnalyzer(R&& inRanker, const query_analyzer_type& inAnalyzer)
		: parent_type(std::forward<R>(inRanker))
		, mAnalyzer(inAnalyzer)
		{}
		
		template <class R>
		RankerWithAnalyzer(const R& inRanker)
		: parent_type(inRanker)
		, mAnalyzer(inRanker.analyzer())
		{}

		template <class R>
		RankerWithAnalyzer(R&& inRanker)
		: parent_type(std::forward<R>(inRanker))
		, mAnalyzer(inRanker.analyzer())
		{}

		RankerWithAnalyzer() {}
		
		using parent_type::operator();
		
		result_type		operator() (const query_type& query) {
      return parent_type::operator()(mAnalyzer(query));
    }
		
		query_analyzer_type& analyzer() { return mAnalyzer; } 
		const query_analyzer_type& analyzer() const { return mAnalyzer; }
		
	private:
		query_analyzer_type	mAnalyzer;
	};

	namespace i {
		
		
		/** ----------------------------------------------------------------------------
		 *
		 */
		
		template <class R>
		struct Classifier : public ParameterizedHelper<Classifier<R>, R> {
      
			typedef ParameterizedHelper<Classifier<R>, R>	parent_type;
			typedef R										ranker_type;

			JEROME_INTERNAL_RANKER_TYPES(R)
			
			result_type	operator() (const argument_type& query) {
				return filterResult(parent_type::operator()(query));
			}
			
			double threshold() const { return mThreshold.value(); }
			void setThreshold(double value) { mThreshold.value() = value; }
			
      using parent_type::parent_type;
			
			template <class ...Args>
			Classifier(Args ...args, double inThreshold = 0)
			: parent_type(args...)
			, mThreshold(inThreshold) {}

		protected:

			result_type filterResult(result_type&& base) {
				if (mThreshold.value() >= 0) return base;
				
				result_type	result(base.size());
				for(std::size_t i = 0, n = base.size(); i < n; ++i) {
					for(const typename ranked_list_type::value_type& j : base[i]) {
						if (j.score() > mThreshold.value()) result[i].push_back(j);
					}
				}
				return result;
			}
				
			ranker_type& base_ranker() { return *this; }
			const ranker_type& base_ranker() const { return *this; }
			
		private:
      
      class Threshold : public Parameter {
      public:
        using Parameter::Parameter;
        Threshold(double inThreshold = 0)
        : Parameter("threshold", inThreshold, -20, 0) {}
        
      };
      
			Threshold	mThreshold;

			friend parent_type;
			template <class Visitor, class Visitable>
			static void visit(Visitor& visitor, Visitable& visitable) {
				visitable.base_ranker().accept(visitor);
				visitable.mThreshold.accept(visitor);
			}
			
		};
		
	}
			
}} // namespace jerome::ir

#endif // defined __jerome_ir_ranker_hpp__
