//
//  async_gcd.hpp
//
//  Created by Anton Leuski on 8/5/15.
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

#ifndef __jerome_async_gcd_hpp__
#define __jerome_async_gcd_hpp__

# if !OS_OBJECT_HAVE_OBJC_SUPPORT
// #warning "redefining __bridge"
// #define __bridge_transfer
#  define __bridge_retained
#  define JEROME_BRIDGE_DEFINED
# endif

# include <dispatch/dispatch.h>

namespace jerome {
namespace dispatch {

namespace detail {

class QueueImplementation
{

public:
  QueueImplementation(const char* inLabel, queue::kind_type inKind)
    : gcdQueue((__bridge_retained dispatch_queue_t)dispatch_queue_create(
        inLabel,
        inKind == queue::SERIAL ? DISPATCH_QUEUE_SERIAL : DISPATCH_QUEUE_CONCURRENT))
  {
  }

  ~QueueImplementation()
  {
    if (!gcdQueue) return;
# ifndef JEROME_BRIDGE_DEFINED
    dispatch_queue_t  q __attribute__((unused)) =
      (__bridge_transfer dispatch_queue_t)gcdQueue;
# else
    dispatch_release(gcdQueue);
# endif
    gcdQueue = nullptr;
  }

  const char * label() const
  {
    return dispatch_queue_get_label(gcdQueue);
  }

  void call(operation function)
  {
    dispatch_async(gcdQueue, ^ { function(); });
  }

private:
  friend class GroupImplementation;
  dispatch_queue_t  gcdQueue;

};

class GroupImplementation
{
public:
  GroupImplementation()
    : gcdGroup((__bridge_retained dispatch_group_t)dispatch_group_create())
  {
  }

  ~GroupImplementation()
  {
    if (!gcdGroup) return;
# ifndef JEROME_BRIDGE_DEFINED
    dispatch_group_t  q __attribute__((unused)) =
      (__bridge_transfer dispatch_group_t)gcdGroup;
# else
    dispatch_release(gcdGroup);
# endif
    gcdGroup = nullptr;
  }

  void call(queue& queue, operation function)
  {
    dispatch_group_async(gcdGroup,
      queue.implementation().gcdQueue, ^ { function(); });
  }

  void wait()
  {
    dispatch_group_wait(gcdGroup, DISPATCH_TIME_FOREVER);
  }

  bool wait_for(uint64_t timeout_in_microseconds)
  {
    dispatch_time_t timeout;
    if (timeout_in_microseconds == time::NOW)
      timeout = DISPATCH_TIME_NOW;
    else
      timeout =
        dispatch_time(DISPATCH_TIME_NOW, timeout_in_microseconds * 1000);

    return 0 == dispatch_group_wait(gcdGroup, timeout);
  }

private:
  dispatch_group_t  gcdGroup;

};


class TimerImplementation : std::enable_shared_from_this<TimerImplementation>
{

public:
  TimerImplementation(const queue& queue,
    time::type repeat_interval_in_microseconds,
    const timer::callback_type& callback,
		bool inRepeats)
  {
  }
	
  void start()
  {
  }

  void stop()
  {
  }

private:
//  typedef boost::asio::deadline_timer   timer_type;
//  timer_type mTimer;
//	queue	mQueue;
//	time::type mInterval;
//	timer::callback_type	mCallback;
//	bool mRepeats;

};

}

}
}

# ifdef JEROME_BRIDGE_DEFINED
#  undef JEROME_BRIDGE_DEFINED
#  undef __bridge_retained
# endif

#endif // defined __jerome_async_gcd_hpp__
