//
//  async.cpp
//
//  Created by Anton Leuski on 11/12/12.
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
