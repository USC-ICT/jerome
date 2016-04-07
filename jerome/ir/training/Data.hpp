//
//  data.hpp
//
//  Created by Anton Leuski on 7/31/15.
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

#ifndef __jerome_ir_training_Data_hpp__
#define __jerome_ir_training_Data_hpp__

#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <jerome/types.hpp>

namespace jerome {
  namespace ir {
    namespace training {

      // random generator function:
      inline ptrdiff_t myrandom(ptrdiff_t i)
      {
        return rand() % i;
      }

      template <class Q, class A, class L>
      struct DataStorage
      {
        typedef Q question_type;
        typedef A answer_type;
        typedef L link_type;

        typedef List<question_type> questions_type;
        typedef List<answer_type> answers_type;
        typedef List<link_type>   links_type;

        questions_type  mQuestions;
        answers_type  mAnswers;
        links_type  mLinks;

        DataStorage() = default;

        template <class QR, class AR, class LR>
        DataStorage(const QR& qr, const AR& ar, const LR& lr)
          : mQuestions(std::begin(qr), std::end(qr))
          , mAnswers(std::begin(ar), std::end(ar))
          , mLinks(std::begin(lr), std::end(lr))
        {
        }

      };

      template <class Q, class A, class L>
      struct Data
        : public ReferenceClassInterface<DataStorage<Q, A, L>>
      {
        typedef DataStorage<Q, A, L> storage_type;

        typedef typename storage_type::question_type question_type;
        typedef typename storage_type::answer_type answer_type;
        typedef typename storage_type::link_type link_type;

        typedef ReferenceClassInterface<storage_type> parent_type;
        typedef Data<question_type, answer_type, link_type>   this_type;

        typedef typename storage_type::questions_type questions_type;
        typedef typename storage_type::answers_type answers_type;
        typedef typename storage_type::links_type links_type;

        template <class QR, class AR, class LR>
        Data(const QR& qr, const AR& ar, const LR& lr)
          : parent_type(std::make_shared<storage_type>(qr, ar, lr))
        {
        }

        Data()
          : parent_type(std::make_shared<storage_type>())
        {
        }

        using parent_type::parent_type;

        questions_type&     questions()
        {
          return this->implementation().mQuestions;
        }

        answers_type&     answers()
        {
          return this->implementation().mAnswers;
        }

        links_type&       links()
        {
          return this->implementation().mLinks;
        }

        const questions_type& questions() const
        {
          return this->implementation().mQuestions;
        }

        const answers_type&   answers() const
        {
          return this->implementation().mAnswers;
        }

        const links_type&   links()   const
        {
          return this->implementation().mLinks;
        }

      private:
        typedef Set<question_type>  question_set_type;

      public:
        typedef double split_size_type;

        template <class Range>
        this_type subdata(const Range& inQuestionRange) const
        {

          question_set_type questions_set(std::begin(inQuestionRange), std::end(
              inQuestionRange));
          auto check_question =
            make_object_contained_in_set<question_set_type, question_type>(
              questions_set);

          this_type newData;

          boost::range::push_back(newData.answers(), answers());
          boost::range::push_back(newData.questions(), inQuestionRange);
          boost::range::push_back(newData.links(), links()
            | boost::adaptors::filtered((std::function<bool(const link_type&)> )(
                [&check_question] (const link_type& l)
          {
            return l.utterances().end() !=
            boost::find_if(l.utterances(), check_question);
          })));
          return newData;
        }

        this_type dataByRemovingUnlinkedQuestions() const
        {
          question_set_type linked_questions_set; // all questions that have
                                                  // links
          question_set_type all_questions_set(std::begin(questions()), std::end(
              questions()));
          for (const link_type& l : links()) {
            boost::range::push_back(linked_questions_set, l.utterances()
              | boost::adaptors::filtered(make_object_contained_in_set(
                  all_questions_set)));
          }

          this_type newData;
          boost::range::push_back(newData.answers(), answers());
          boost::range::push_back(newData.questions(), linked_questions_set);
          boost::range::push_back(newData.links(), links());
          return newData;
        }

        // put a proportion of the questions into the first data set and the
        // rest into the second set
        // if the proportion is weird (< 0 or >= |Q|) then duplicate the set
        //

        template <class RandomNumberGenerator>
        std::pair<this_type, this_type> split(split_size_type inProportion,
                                              RandomNumberGenerator& inRandomNumberGenerator)
        const
        {

          std::size_t maxCount  = questions().size();
          std::size_t count   = (inProportion < 0 || inProportion >= maxCount)
                                ? maxCount
                                : (std::size_t)((inProportion < 1)
                                                ? inProportion * maxCount
                                                : inProportion);

          if (count == maxCount) {
            return std::pair<this_type, this_type>(*this, *this);
          } else {
            std::vector<question_type>  randomized_questions(
              questions().begin(), questions().end());
            boost::range::random_shuffle(randomized_questions,
              inRandomNumberGenerator);

            this_type firstData =
              subdata(boost::make_iterator_range(randomized_questions.begin(),
                  randomized_questions.begin() + count));
            this_type secondData  =
              subdata(boost::make_iterator_range(randomized_questions.begin() +
                  count, randomized_questions.end()));

            return std::pair<this_type, this_type>(firstData, secondData);
          }

        }

        std::pair<this_type,
          this_type> split(split_size_type inProportion) const
        {
          ptrdiff_t (* p_myrandom)(ptrdiff_t) = myrandom;
          return split(inProportion, p_myrandom);
        }

      };

    }
  }
}
#endif // defined __jerome_ir_training_Data_hpp__