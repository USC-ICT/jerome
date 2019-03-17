//
//  lm.hpp
//
//  Created by Anton Leuski on 10/1/11.
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

#ifndef __jerome_ir_rm_rm_hpp__
#define __jerome_ir_rm_rm_hpp__

#include <vector>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#include <jerome/types.hpp>
#include <jerome/ir/collection.hpp>
#include <jerome/ir/ranker.hpp>
#include <jerome/ir/analysis.hpp>
#include <jerome/ir/rm/weighting.hpp>

namespace jerome { namespace ir { namespace rm {

	using namespace jerome::math;
	using namespace jerome::math::parameters;
	
#pragma mark -
#pragma mark RelevanceModel
	
	template <class Index>
	struct Context : public weighting::Context<Index> {
		Context(const Index& inIndex)
		: mIndex(inIndex) {}
		const Index&			index() const { return mIndex; }
	private:
		const Index&			mIndex;
	};

//	template <class QW, class DW>
//	struct Weighting : public ParameterizedHelper<Weighting<QW, DW>> {
//		typedef ParameterizedHelper<Weighting<QW, DW>>		parent_type;
//		typedef QW											query_weighting_type;
//		typedef DW											document_weighting_type;
//		
//		Weighting()
//		{}
//
//		const document_weighting_type&	documentWeightings() const { return mDocumentWeighting; }
//		const query_weighting_type&		queryWeightings()	 const { return mQueryWeighting; }
//		
//		document_weighting_type&	documentWeightings() { return mDocumentWeighting; }
//		query_weighting_type&		queryWeightings()	 { return mQueryWeighting; }
//
//	private:
//		
//		document_weighting_type			mDocumentWeighting;
//		query_weighting_type			mQueryWeighting;
//		
//		friend parent_type;
//		template <class Visitor, class Visitable> static void visit(Visitor& visitor, Visitable& visitable) {
//			visitable.documentWeightings().accept(visitor);
//			visitable.queryWeightings().accept(visitor);
//		}
//	};
	
	template <class Index>
	struct DocumentModel {
		typedef	boost::counting_iterator<int>	object_iterator_type;
		typedef Index	index_type;
		typedef Context<index_type> context_type;
		
		DocumentModel()
		: mIndex()
		, mContext(mIndex)
		{
		}
		
		DocumentModel(const DocumentModel& inOther)
		: mIndex(inOther.mIndex)
		, mContext(mIndex)
		{}
		
		template <class Range, class Analyzer>
		void index(const Range& range, Analyzer analyzer)
		{
			for(const auto& d : range) {
				analyzer.parse(d, mIndex);
			}
			mIndex.optimize();
		}
		
		index_type& index() { return mIndex; }
		const index_type& index() const { return mIndex; }

		const context_type& context() const { return mContext; }
		object_iterator_type objectIterator() const { return boost::counting_iterator<int>(0); }

	private:
		context_type		mContext;
		index_type			mIndex;
	};
	
	template <typename D, class Index>
	struct StoredDocumentModel : public DocumentModel<Index> {
		typedef List<D> object_list_type;
		typedef typename object_list_type::const_iterator	object_iterator_type;
		
		StoredDocumentModel()
		: DocumentModel<Index>()
		{
		}

		using DocumentModel<Index>::index;
		
		template <class Range, class Analyzer>
		void index(const Range& range, Analyzer analyzer)
		{
			DocumentModel<Index>::index(range, analyzer);
			boost::push_back(mObjects, range);
		}

		object_list_type& objects() { return mObjects; }
		const object_list_type& objects() const { return mObjects; }
		
		object_iterator_type objectIterator() const { return mObjects.begin(); }

	private:
		object_list_type	mObjects;
	};
	
	template <typename O, typename W, typename I, typename M>
	struct Desc {
		typedef O		object_type;
		typedef W		weighting_type;
		typedef I		index_type;
		typedef M		model_type;
		typedef typename model_type::context_type context_type;
		
		weighting_type& weightings() { return mWeightings; }
		const weighting_type& weightings() const { return mWeightings; }

		index_type& index() { return model().index(); }
		const index_type& index() const { return model().index(); }

		model_type&	model() { return mModel; }
		const model_type& model() const { return mModel; }
		
		const context_type& context() const { return model().context(); }

	private:
		weighting_type	mWeightings;
		model_type		mModel;
	};
	
	template <typename Q, typename D, typename RM>
	struct Ranker : public ParameterizedHelper<Ranker<Q,D,RM>,
							jerome::ir::i::Ranker<typename Q::object_type,
									typename D::object_type>>
	{
		
	private:
		typedef jerome::ir::i::Ranker<typename Q::object_type, typename D::object_type> _RT;
		
	public:
		typedef ParameterizedHelper<Ranker<Q,D,RM>, _RT>		parent_type;
		typedef Ranker<Q,D,RM>									this_type;
		
		JEROME_INTERNAL_RANKER_TYPES(_RT)
						
		Ranker()
		{
		}
		
		void index(
				   const typename Q::weighting_type& inQueryWeighting,
				   const typename D::weighting_type& inDocumentWeighting)
		{
			query().weightings()	= inQueryWeighting;
			document().weightings()	= inDocumentWeighting;
		}
		
		result_type	operator() (const query_type& inQuery) {
			const WeightMatrix&	qm	= this->queryModel(inQuery);
//			std::cout << inQuery << " -- " << qm << std::endl;
//			std::cout << inQuery << " " << std::endl;
			return postRank(this->rank(qm));
		}
				
		using parent_type::accept;
		
		void accept(SetValue& ioVisitor) {
			visit(ioVisitor, *this);
			clearCache(); // not thread safe
		}

//		std::size_t countOfDocuments() const { return document().model().countOfDocuments(); }
//		std::size_t countOfQueries()   const { return query().model().countOfQueries(); }

		template <class Index>
		WeightMatrix	queryModel(const Index& inQuery) {
			return query().weightings()
        .computeQueryModelForQuery(model().queryWeightingContext(), inQuery);
		}
		
		Q& query() { return mQuery; }
		const Q& query() const { return mQuery; }
		D& document() { return mDocument; }
		const D& document() const { return mDocument; }
		
	protected:
		
		void clearCache() {
			mAffinityMatrix = jerome::shared_ptr<FastMatrix>();
		}
		
		FastMatrix	documentWeight(const WeightMatrix& inQueryModel) {
			if (nullptr == mAffinityMatrix.get()) {
#ifdef VIENNACL_WITH_OPENCL
				WeightMatrix	tmp = documentWeightings().computeAffinity(model().documentWeightingContext());
				mAffinityMatrix = jerome::shared_ptr<FastMatrix>(new FastMatrix(tmp.size1(), tmp.size2()));
				viennacl::copy(tmp, *mAffinityMatrix);
#else
				auto x = std::make_shared<FastMatrix>(
            document().weightings().computeAffinity(
              model().documentWeightingContext()));
				mAffinityMatrix = x;
				return *x;
#endif
			}
			return *mAffinityMatrix;
		}
		
		WeightMatrix	initialWeight(const WeightMatrix& inQueryModel) {
			return document().weightings().computeAffinityInitialWeight(
                      model().documentWeightingContext(), inQueryModel);
		}

				
		result_type 	rank(const WeightMatrix& inQueryModel) {
			return this->rankDocuments(inQueryModel, document().model().objectIterator());
		}
		
		template <class DocumentIterator>
		result_type 	rankDocuments(const WeightMatrix& inQueryModel, DocumentIterator inDocIterator) {
			const FastMatrix&				documentWeight	= this->documentWeight(inQueryModel);
			const WeightMatrix&				initialWeight	= this->initialWeight(inQueryModel);
			
      MatrixSize queryModelSize(inQueryModel);
			result_type						ranked_list(queryModelSize.columnCount);
			
			//					std::cout << inQueryModel.size() << " " << documentWeight->size1() << " " << documentWeight->size2() << " " << initialWeight->size() << std::endl;
			
#ifdef VIENNACL_WITH_OPENCL
			FastMatrix vcl_b(inQueryModel.size1(), inQueryModel.size2());
			viennacl::copy(inQueryModel, vcl_b);
			FastMatrix vcl_c(initialWeight.size1(), initialWeight.size2());
			viennacl::copy(initialWeight, vcl_c);
			
			FastMatrix vcl_d = viennacl::linalg::prod(documentWeight,vcl_b) + vcl_c;
			
			WeightMatrix	scores(vcl_d.size1(), vcl_d.size2());
			viennacl::copy(vcl_d, scores);
#else
      WeightMatrix	scores = model().prod(documentWeight, inQueryModel) + initialWeight;
#endif
      
      MatrixSize scoresSize(scores);
			for(std::size_t i = 0, n = scoresSize.rowCount; i < n; ++i) {
				auto theDoc(*inDocIterator++);
				for(std::size_t j = 0, m = scoresSize.columnCount; j < m; ++j) {
					ranked_list[j].push_back(typename ranked_list_type::value_type(scores(i, j), theDoc));
				}
			}
      
      return ranked_list;
		}
		
		result_type	postRank(result_type inRankedList) {
			for(auto & rl : inRankedList) {
				std::sort(rl.begin(), rl.end(), ScoresDescending());
			}
			//std::cout << inRankedList.size() << " + " << inRankedList[0].size() << " " << inRankedList[0] << std::endl;
			return inRankedList;
		}
		
	private:
		Q	mQuery;
		D	mDocument;
		jerome::shared_ptr<FastMatrix>	mAffinityMatrix;
				
		RM& model() { return *static_cast<RM*>(this); }
		const RM& model() const { return *static_cast<const RM*>(this); }
		
		friend parent_type;
		template <class Visitor, class Visitable> static void visit(Visitor& visitor, Visitable& visitable) {
			visitable.document().weightings().accept(visitor);
			visitable.query().weightings().accept(visitor);
		}
	};

	
	
}}}

#endif // defined __jerome_ir_rm_rm_hpp__
