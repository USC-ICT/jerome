//
//  collection.hpp
//
//  Created by Anton Leuski on 7/28/14.
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

#ifndef __jerome_ir_rm_weighting_document_collection_hpp__
#define __jerome_ir_rm_weighting_document_collection_hpp__

#include <jerome/ir/rm/weighting.hpp>

namespace jerome { namespace ir { namespace rm { namespace weighting { namespace document {

#pragma mark - Document
	
	struct Document  {
		//					virtual WeightMatrix computeAffinity(const Context& inContext) const = 0;
		//					virtual WeightMatrix computeAffinityInitialWeight(const Context& inContext, const WeightMatrix& inModel) const = 0;
	};

#pragma mark - DocumentCollection
	
	template <class... Args>
	struct DocumentCollection; //  : public Collection<Document, Args...> {};
	
	template <class Arg1>
	struct DocumentCollection<Arg1> : public Collection<Document, Arg1> {
		typedef DocumentCollection<Arg1>	this_type;
		typedef Collection<Document, Arg1>			parent_type;
		using parent_type::parent_type;
		
		template <class Index>
		WeightMatrix computeAffinity(const Context<Index>& inContext) const { return std::get<0>(this->weightings()).computeAffinity(inContext); }
		template <class Index>
		WeightMatrix computeAffinityInitialWeight(const Context<Index>& inContext, const WeightMatrix& inModel) const { return std::get<0>(this->weightings()).computeAffinityInitialWeight(inContext, inModel); }
	};
	
	template <class Arg1, class Arg2, class... Args>
	struct DocumentCollection<Arg1, Arg2, Args...> : public Collection<Document, Arg1, Arg2, Args...> {
		typedef DocumentCollection<Arg1, Arg2, Args...>	this_type;
		typedef Collection<Document, Arg1, Arg2, Args...>			parent_type;
		using parent_type::parent_type;
	};

#pragma mark - AddDocumentProbabilities
	
  struct AddDocumentProbabilitiesConst {
    static constexpr const char* IDENTIFIER =
    "jerome.weighting.document.add";
  };
  
	template <class... Args>
	struct AddDocumentProbabilities
    : public DocumentCollection<Args...>
    , public AddDocumentProbabilitiesConst
  {
		typedef AddDocumentProbabilities<Args...>	this_type;
		typedef DocumentCollection<Args...>			parent_type;
		using parent_type::parent_type;
	};
	
	template <class Arg1>
	struct AddDocumentProbabilities<Arg1>
    : public DocumentCollection<Arg1>
    , public AddDocumentProbabilitiesConst
  {
		typedef AddDocumentProbabilities<Arg1>	this_type;
		typedef DocumentCollection<Arg1>			parent_type;
		using parent_type::parent_type;

		Record model() const
		{
			Record record = parent_type::model();
			record.emplace_front(Weighting::NAME_KEY, this->name());
			record.emplace_front(Weighting::IDENTIFIER_KEY, IDENTIFIER);
			return record;
		}
		
	};
	
	namespace detail {
		
		template <class Index>
		inline MatrixSize	affinityMatrixSize(const Context<Index>& inContext) {
			auto ndocs = inContext.index().documentCount();
      MatrixSize size;
      size.rowCount     = ndocs;
      size.columnCount  = ndocs;
			return size;
		}
		
		template <class Index>
		inline MatrixSize	affinityInitialWeightMatrixSize(const Context<Index>& inContext, const WeightMatrix& inModel) {
      MatrixSize size(inModel);
      size.rowCount = inContext.index().documentCount();
			return size;
		}
		
		template <int N, class this_type>
		struct add_document_helper {
			template <class Index>
			static WeightMatrix computeAffinity(const this_type& sup, const Context<Index>& inContext) {
				return sup.alphas()[this_type::size-N] * std::get<this_type::size-N>(sup.weightings()).computeAffinity(inContext) +
				add_document_helper<N-1, this_type>::computeAffinity(sup, inContext);
			}
			template <class Index>
			static WeightMatrix computeAffinityInitialWeight(const this_type& sup, const Context<Index>& inContext, const WeightMatrix& inModel) {
				return sup.alphas()[this_type::size-N] * std::get<this_type::size-N>(sup.weightings()).computeAffinityInitialWeight(inContext, inModel) +
				add_document_helper<N-1, this_type>::computeAffinityInitialWeight(sup, inContext, inModel);
			}
		};
		
		template <class this_type>
		struct add_document_helper<0, this_type> {
			template <class Index>
			static auto computeAffinity(const this_type& sup, const Context<Index>& inContext)
      -> decltype(WeightMatrixZero(affinityMatrixSize(inContext)))
      {
				return WeightMatrixZero(affinityMatrixSize(inContext));
			}
			template <class Index>
			static auto computeAffinityInitialWeight(const this_type& sup, const Context<Index>& inContext, const WeightMatrix& inModel)
      -> decltype(WeightMatrixZero(affinityInitialWeightMatrixSize(inContext, inModel)))
      {
				return WeightMatrixZero(affinityInitialWeightMatrixSize(inContext, inModel));
			}
		};
		
	}
	
	template <class Arg1, class Arg2, class... Args>
	struct AddDocumentProbabilities<Arg1, Arg2, Args...> : public DocumentCollection<Arg1, Arg2, Args...> {
		typedef AddDocumentProbabilities<Arg1, Arg2, Args...>	this_type;
		typedef DocumentCollection<Arg1, Arg2, Args...>			parent_type;
		using parent_type::parent_type;
		
		const static std::size_t size = parent_type::size;

		static constexpr const char* IDENTIFIER =
			"jerome.weighting.document.add";
		
		Record model() const
		{
			Record record = parent_type::model();
			record.emplace_front(Weighting::NAME_KEY, this->name());
			record.emplace_front(Weighting::IDENTIFIER_KEY, IDENTIFIER);
			return record;
		}
		
		template <class Index>
		WeightMatrix computeAffinity(const Context<Index>& inContext) const {
			return detail::add_document_helper<size, this_type>::computeAffinity(*this, inContext);
		}
		
		template <class Index>
		WeightMatrix computeAffinityInitialWeight(const Context<Index>& inContext, const WeightMatrix& inModel) const {
			return detail::add_document_helper<size, this_type>::computeAffinityInitialWeight(*this, inContext, inModel);
		}
		
	};

}}}}}

#endif // defined __jerome_ir_rm_weighting_document_collection_hpp__
