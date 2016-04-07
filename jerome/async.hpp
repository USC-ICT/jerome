//
//  async.hpp
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
