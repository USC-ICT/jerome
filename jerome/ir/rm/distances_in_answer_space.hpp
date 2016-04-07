//
//  distances_in_answer_space.hpp
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

#ifndef __jerome_ir_rm_distances_in_answer_space_hpp__
#define __jerome_ir_rm_distances_in_answer_space_hpp__

#include <jerome/ir/distances.hpp>

namespace jerome { namespace ir { namespace rm {
	
	template <class Ranker, class Index>
	struct DistanceComputationInAnswerSpace {
		
		static bool compute(Ranker& inRanker, DistanceComputationDelegate& ioDelegate) {
			
			const std::size_t	ndocs	= inRanker.countOfDocuments();
			const std::size_t	nqrys	= inRanker.countOfQueries();
			
			/*
			 distance(x, y) = sqrt( d(x||y) + d(y||x) )
			 */
			
			ioDelegate.setObjectCount(ndocs+nqrys);
			
			int		stateProgressWeights[]		= { 1, 1, 1, 100 };
			int		stateProgressWeightsCount	= sizeof(stateProgressWeights) / sizeof(*stateProgressWeights);
			double	totalStateProgressWeights	= 0;
			for(int i = 0; i < stateProgressWeightsCount; ++i)
				totalStateProgressWeights += stateProgressWeights[i];
			int		stateProgressStage			= 0;
			double	progress					= 0;
			
			if (!ioDelegate.noteProgress(progress)) return false;
			
			const WeightMatrix&	A		= inRanker.documentWeightings().computeAffinity(inRanker.documentWeightingContext());
			const WeightMatrix&	Ae		= inRanker.expandDocumentMatrix(A);
			
			progress += stateProgressWeights[stateProgressStage++] / totalStateProgressWeights;
			if (!ioDelegate.noteProgress(progress)) return false;
			
			const WeightMatrix&	B		= inRanker.queryModel(inRanker.queryWeightingContext().index());
			
			progress += stateProgressWeights[stateProgressStage++] / totalStateProgressWeights;
			if (!ioDelegate.noteProgress(progress)) return false;
			
			DistanceComputationDelegate& R = ioDelegate;
			
			/*
			 A(i,j) = Sum_x ( D_j(x) log D_i(x) )
			 Q_i(x) = Sum_p ( D_p(x) B(p,i) ), where p is a linked pair D,Q
			 Ae ~ A, but Ae(i,j) = Sum_x ( D_p_j(x) log D_i(x) ), p_j is the jth linked pair
			 
			 
			 d(D_i||D_j) = Sum_x ( D_i(x) log D_i(x) - D_i(x) log D_j(x) + D_j(x) log D_j(x) - D_j(x) log D_i(x) )
			 = A(i,i) - A(j,i) + A(j,j) - A(i,j)
			 
			 
			 d(D_i||Q_j) = Sum_x ( D_i(x) log D_i(x)		- D_i(x) log Q_j(x)				+ Q_j(x) log Q_j(x)				- Q_j(x) log D_i(x) )
			 = Sum_x ( D_i(x) log D_i(x) )	- Sum_x ( D_i(x) log Q_j(x) )	+ Sum_x ( Q_j(x) log Q_j(x) )	- Sum_x( Q_j(x) log D_i(x) )
			 
			 Sum_x( Q_j(x) log D_i(x) ) =
			 = Sum_x( Sum_p ( D_p(x) B(p,j) ) log D_i(x) )
			 = Sum_p( B(p,j) Sum_x( D_p(x) log D_i(x) ) )
			 = Sum_p( B(p,j) A(i,p) )
			 = row(Ae,i) . column(B,j)
			 
			 = A(i,i)						-  Sum_x ( D_i(x) log Q_j(x) )	+ Sum_x ( Q_j(x) log Q_j(x) )	- row(Ae,i) . column(B,j)
			 = A(i,i) - row(Ae,i) . column(B,j) + Sum_x ( (Q_j(x) - D_i(x)) log Q_j(x) )
			 
			 d(Q_i||Q_j) = Sum_x ( Q_i(x) log Q_i(x) - Q_i(x) log Q_j(x) + Q_j(x) log Q_j(x) - Q_j(x) log Q_i(x) )
			 = Sum_x ( (Q_i(x) - Q_j(x)) * (log Q_i(x) - log Q_j(x)) )
			 = Sum_x (
			 
			 Sum_x( Q_i(x) log Q_i(x) ) = Sum_x ( C(x)_i log C(x)_j )
			 C(x)_k = Q_k(x) = Sum_p ( D_p(x) B(p,k) )
			 
			 */
			SymmetricWeightMatrix			D(ndocs+nqrys, ndocs+nqrys);
			SymmetricWeightMatrixRange		DD(D, jerome::Range(0,ndocs), jerome::Range(0,ndocs));
			SymmetricWeightMatrixRange		QQ(D, jerome::Range(ndocs,ndocs+nqrys), jerome::Range(ndocs,ndocs+nqrys));
			SymmetricWeightMatrixRange		DQ(D, jerome::Range(0,ndocs), jerome::Range(ndocs,ndocs+nqrys));
			
			WeightVector			diagA = diag(A);
			WeightScalarVector		Ed(ndocs, 1);
			WeightScalarVector		Eq(nqrys, 1);
			
			DD = outer_prod(diagA, Ed) - A + outer_prod(Ed, diagA) - trans(A);
			
			//					for(std::size_t i = 0, n = DD.size1(); i < n; ++i) {
			//						for(std::size_t j = 0; j <= i; ++j) {
			//							DD(i,j) = A(i,i) - A(i,j) + A(j,j) - A(j,i);
			//						}
			//					}
			
			DQ = outer_prod(diagA, Eq) - prod(Ae, B);
			
			//					for(std::size_t i = 0, n = nqrys; i < n; ++i) {
			//						WeightMatrixConstColumn	column(B, i);
			//						for(std::size_t j = 0; j < ndocs; ++j) {
			//							WeightMatrixConstRow	a(A, j);
			//							R(i+ndocs,j) += A(j,j) - mDocumentContext.dotOperator()(column, a);
			//						}
			//					}
			
			auto&			documentWeighting = inRanker.documentWeightings();
			const Index&	docIndex(inRanker.documentWeightingContext().index());
			const typename Index::Field&	field	= docIndex.findField(documentWeighting.name());
			
			progress += stateProgressWeights[stateProgressStage++] / totalStateProgressWeights;
			if (!ioDelegate.noteProgress(progress)) return false;
			
			int		count = 0;
			
			for(const auto& te : field.terms()) {
				const typename Index::Term&			term(te.second);
				const double		col_stat_a	= documentWeighting.collection_weight(term, field);
				WeightVector		C(nqrys, col_stat_a);
				WeightVector		logC(nqrys, 0);
				
				SparseWeightVector	af	= documentWeighting.document_weight(term, field);
				
				axpy_prod(inRanker.expandDocumentVector(af), B, C, false); // C += afe x trans(B)
				
				logC = log(C);
				
				QQ += element_prod(outer_prod(C, Eq) - outer_prod(Eq, C), outer_prod(logC, Eq) - outer_prod(Eq, logC));

//				WeightMatrix x1 = outer_prod(af+WeightScalarVector(ndocs,col_stat_a), Eq);
//				WeightMatrix x2 = outer_prod(Ed, C);
//				WeightMatrix x3 = outer_prod(Ed, logC);
//				WeightMatrix x4 = element_prod(x2 - x1, x3);
//				DQ += x4;
				
				DQ += element_prod(outer_prod(Ed, C) - outer_prod(af+WeightScalarVector(ndocs,col_stat_a), Eq), outer_prod(Ed, logC));
				
				
				//						for(std::size_t i = 0, n = nqrys; i < n; ++i) {
				//							for(std::size_t j = 0; j <= i; ++j) {
				//								R(i+ndocs,j+ndocs) += (C(i) - C(j)) * (logC(i)-logC(j));
				//							}
				//							for(std::size_t j = 0; j < ndocs; ++j) {
				//								R(i+ndocs,j) += (C(i) - (col_stat_a + af(j))) * logC(i);
				//							}
				//						}
				if (!ioDelegate.noteProgress(progress + ((++count) / (double)field.terms().size()) * (stateProgressWeights[stateProgressStage] / totalStateProgressWeights) )) return false;
			}
			
			for(std::size_t i = 0, n = D.size1(); i < n; ++i) {
				for(std::size_t j = 0; j <= i; ++j) {
					R(i,j) = sqrt(fabs(D(i,j)));
					//							R(i,j) = sqrt(fabs(R(i,j))); // the distance is a sqrt(KL(Q|D) + KL(D|Q)) I'm taking abs() just in case... :)
				}
			}
			
			ioDelegate.noteProgress(1);
			return true;
		}
		
	};

}}}

#endif // defined __jerome_ir_rm_distances_in_answer_space_hpp__
