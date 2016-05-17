//
//  distances_in_mixed_space.hpp
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

#ifndef __jerome_ir_rm_distances_in_mixed_space_hpp__
#define __jerome_ir_rm_distances_in_mixed_space_hpp__

#include <jerome/ir/distances.hpp>

// this is a weird formula: I compute the distances between queries using the same technique
// (weights) as the distances between documents but in query space. I calculate the distances
// between documents and between documents and queries using the main RM approach, so they
// are computed in answer space. That way the distances are actually from two different spaces
// and should not be comparable.
//
// However, if you think about it, is sort of makes sense: I visualize distances among
// documents, then I visualize how queries relate to the documents and visualize how queries relate
// to each other.
//
// Also, were I to compute query-query distances as distances between query objects
// in document space (see distances_in_answer_space), the distances between queries
// are lowered (artificially?) because of the links to documents.

// if I'm to visualize the distances -- the query-query distances will

namespace jerome { namespace ir { namespace rm {

	template <class Ranker>
	struct DistanceComputationInMixedSpace {
		
		static bool compute(Ranker& inRanker, DistanceComputationDelegate& ioDelegate) {
			
			double progress = 0;
			if (!ioDelegate.noteProgress(progress)) return false;
			
			const std::size_t	ndocs	= inRanker.countOfDocuments();
			const std::size_t	nqrys	= inRanker.countOfQueries();
			
			double	totalProgress
			= 1 // computeAffinity for documents
			+ 1 // computeQueryModelsForAllQueries
			+ 1 // computeAffinity
			+ 1 // compute distances for docs
			+ 1 // compute distances for queries
			+ nqrys;
			
			double	deltaProgress	= 1/totalProgress;
			
			ioDelegate.setObjectCount(ndocs+nqrys);
			
			const WeightMatrix&	A		= inRanker.document().weightings().computeAffinity(inRanker.documentWeightingContext());
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			const WeightMatrix&	B		= inRanker.queryModel(inRanker.queryWeightingContext().index());
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			const WeightMatrix&	C		= inRanker.document().weightings().computeAffinity(inRanker.queryWeightingContext());
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			SymmetricWeightMatrix			D(ndocs+nqrys, ndocs+nqrys);
			SymmetricWeightMatrixRange		DD(D, jerome::Range(0,ndocs), jerome::Range(0,ndocs));
			SymmetricWeightMatrixRange		QQ(D, jerome::Range(ndocs,ndocs+nqrys), jerome::Range(ndocs,ndocs+nqrys));
			SymmetricWeightMatrixRange		DQ(D, jerome::Range(0,ndocs), jerome::Range(ndocs,ndocs+nqrys));
			WeightVector			diagA = diag(A);
			WeightVector			diagC = diag(C);
			WeightScalarVector		Ed(ndocs, 1);
			WeightScalarVector		Eq(nqrys, 1);
			
			DistanceComputationDelegate& R = ioDelegate;
			
			DD = outer_prod(diagA, Ed) - A + outer_prod(Ed, diagA) - trans(A);
			
//			std::cout << "DD ==========================================================" << std::endl << DD << std::endl;
			
			
			//					for(std::size_t i = 0, n = ndocs; i < n; ++i) {
			//						for(std::size_t j = 0; j <= i; ++j) {
			//							R(i,j) = sqrt(fabs(A(i,i) - A(i,j) + A(j,j) - A(j,i)));
			//						}
			//					}
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			QQ = outer_prod(diagC, Eq) - C + outer_prod(Eq, diagC) - trans(C);
//			std::cout << "QQ ==========================================================" << std::endl << QQ << std::endl;
			
			//					for(std::size_t i = 0, n = nqrys; i < n; ++i) {
			//						for(std::size_t j = 0; j <= i; ++j) {
			//							R(i+ndocs,j+ndocs) = sqrt(fabs(C(i,i) - C(i,j) + C(j,j) - C(j,i)));
			//						}
			//					}
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			DQ = inRanker.prod(A, B);
//			std::cout << "DQ ==========================================================" << std::endl << DQ << std::endl;
			
			//					std::cout << "Ae ==========================================================" << std::endl << Ae << std::endl;
			//
			//					std::cout << "B ==========================================================" << std::endl << B << std::endl;
			
			//					for(std::size_t i = 0, n = nqrys; i < n; ++i) {
			//						WeightMatrixConstColumn	column(B, i);
			//						for(std::size_t j = 0; j < ndocs; ++j) {
			//							WeightMatrixConstRow	a(A, j);
			//							R(i+ndocs,j) = sqrt(fabs(2*mDocumentContext.dotOperator()(column, a)));
			//						}
			//
			//						if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			//					}
			
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

#endif // defined __jerome_ir_rm_distances_in_mixed_space_hpp__
