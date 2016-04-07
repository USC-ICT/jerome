//
//  async.cpp
//
//  Created by Anton Leuski on 11/12/12.
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

#include "async.hpp"

namespace jerome {
namespace dispatch {

const time::type  time::NOW   = 0;
const time::type  time::FOREVER = UINT64_MAX;

}
}

#ifdef JEROME_IOS
// #define JEROME_USE_GCD
#endif

#ifdef JEROME_USE_GCD
# include "async_gcd.hpp"
#else
# include "async_boost.hpp"
#endif

namespace jerome {
namespace dispatch {

queue::queue(kind_type kind, const char* inLabel)
  : parent_type(std::make_shared<detail::QueueImplementation>(inLabel, kind))
{
}

const char * queue::label() const
{
  return implementation().label();
}

void queue::call(operation function)
{
  implementation().call(std::move(function));
}

group::group()
  : parent_type(std::make_shared<detail::GroupImplementation>())
{
}

void group::call(queue& queue, operation function)
{
  implementation().call(queue, std::move(function));
}

bool group::wait_for(time::type timeout_in_microseconds)
{
  if (timeout_in_microseconds == time::FOREVER) {
    wait();
    return true;
  }
  return implementation().wait_for(timeout_in_microseconds);
}

void group::wait()
{
  implementation().wait();
}

timer::timer(const queue& queue,
  time::type repeat_interval_in_microseconds,
  const callback_type& callback,
  bool repeats)
  : parent_type(std::make_shared<detail::TimerImplementation>(queue,
      repeat_interval_in_microseconds, callback, repeats))
{
}

void timer::start()
{
  implementation().start();
}

void timer::stop()
{
  implementation().stop();
}

}
}