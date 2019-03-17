//
//  async.hpp
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

#ifndef __jerome_async_hpp__
#define __jerome_async_hpp__

#include <jerome/types.hpp>

namespace jerome {
  namespace dispatch {

    namespace detail {
      class QueueImplementation;
      class GroupImplementation;
      class TimerImplementation;
    }

    typedef std::function<void ()> operation;

    class queue
      : public ReferenceClassInterface<detail::QueueImplementation>
    {
      typedef ReferenceClassInterface<detail::QueueImplementation> parent_type;

    public:
      enum kind_type {
        SERIAL,
        CONCURRENT
      };

      queue(kind_type kind = SERIAL, const char* inLabel = nullptr);
      const char * label() const;
      void call(operation function);

    private:
      friend struct detail::GroupImplementation;
    };

    struct time
    {
      typedef uint64_t type;
      static const type NOW;
      static const type FOREVER;
    };

// represents a group of operations. Operations submitted to possibly different
// queues are grouped together and you can wait until all operations have
// completed.

    class group
      : public ReferenceClassInterface<detail::GroupImplementation>
    {
      typedef ReferenceClassInterface<detail::GroupImplementation> parent_type;

    public:
      group();
      void call(queue& queue, operation function);
      // returns true if we are done before the timeout
      bool wait_for(uint64_t timeout_in_microseconds);
      void wait();
    };

    class timer
      : public ReferenceClassInterface<detail::TimerImplementation>
    {
      typedef ReferenceClassInterface<detail::TimerImplementation> parent_type;

    public:
      typedef std::function<void (const timer&)> callback_type;
      using parent_type::parent_type;

      timer(const queue& queue,
        time::type repeat_interval_in_microseconds,
        const callback_type& callback,
        bool repeats = false);
      void start();
      void stop();

    private:
      friend class detail::TimerImplementation;
    };

  }
}

#endif // defined __jerome_async_hpp__
