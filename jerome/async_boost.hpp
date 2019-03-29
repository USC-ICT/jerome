//
//  async_boost.hpp
//
//  Created by Anton Leuski on 8/5/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_async_boost_hpp__
#define __jerome_async_boost_hpp__

#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wconstexpr-not-const"
#pragma clang diagnostic ignored "-Wcomma"
#include <boost/asio.hpp>
#pragma clang diagnostic pop

namespace jerome {
  namespace dispatch {

    namespace detail {

      template <typename F>
      inline void callAndCatchExceptions(F&& f)
      {
        try {
          f();
        } catch (const std::exception& ex) {
        std::cerr << "uncaught exception: " << ex.what() << std::endl;
        } catch (...) {
          std::cerr << "unknown uncaught exception" << std::endl;
        }
      }
      
      class Service
      {

      public:
        Service()
          : mCount(0)
          , mIOService()
          , mWork(mIOService)
        {
					addThreads(hardware_concurrency());
        }

				~Service()
				{
					mIOService.stop();
				}

        template <typename Handler>
        void call(boost::asio::io_service::strand* inStrand, Handler&& handler)
        {
          if (inStrand) {
            inStrand->post([handler] { callAndCatchExceptions(handler); } );
          } else {
            incrementOperationCount();
            mIOService.post([this, handler] {
							helper h(*this);
							callAndCatchExceptions(handler);
						});
          }
        }

        static Service& sharedInstance()
        {
          // STATIC
          static Service* service = new Service;
          return *service;
        }

        boost::asio::io_service::strand * makeStrand()
        {
          incrementOperationCount();
          return new boost::asio::io_service::strand(mIOService);
        }

        void destroyStrand(boost::asio::io_service::strand*& strand)
        {
          if (!strand) return;
          decrementOperationCount();
          delete strand;
          strand = nullptr;
        }

        boost::asio::io_service& io_service()
        {
          return mIOService;
        }

      private:
        struct helper
        {
          helper(Service& inService)
            : service(inService)
          {
          }

          ~helper()
          {
            service.decrementOperationCount();
          }

          Service& service;
        };

        void incrementOperationCount()
        {
          ++mCount;
          adjustThreadCountAsNeeded();
        }

        void decrementOperationCount()
        {
          --mCount;
        }

        static std::size_t hardware_concurrency()
        {
          const std::size_t max_thread_count =
            std::thread::hardware_concurrency(); // may return 0
          return max_thread_count > 1 ? max_thread_count : 4;
        }

        void adjustThreadCountAsNeeded()
        {
          static const std::size_t max_thread_count = hardware_concurrency();

          long count = (long)mCount;
          std::size_t needed = count < (long)max_thread_count
                               ? max_thread_count
                               : (std::size_t)(max_thread_count *
                                               (log((double)count) /
                                                log(max_thread_count)));

          if (needed <= mThreadGroup.size()) return;

//    std::cout << std::endl
//			<< "increasing the thread pool to size " << needed << " " << count;

					addThreads(needed - mThreadGroup.size());
        }
				
				void addThreads(const std::size_t count)
				{
          for (std::size_t i = 0; i < count; ++i) {
						std::unique_ptr<std::thread>	t(new std::thread([this] {
							this->mIOService.run();
						}));
						mThreadGroup.push_back(t.release());
					}
				}

        // this counter will increment for every operation posted to
        // the service (not strand) and for every strand created
        // it will decrement when an operation ends and when a strand is
        // destroyed.
        // That way I keep count of the thread load and increase
        // the thread pool accordingly.
        std::atomic_llong                 mCount;
        boost::asio::io_service           mIOService;
        boost::asio::io_service::work     mWork;
        std::list<std::thread*>           mThreadGroup;
      };

      class QueueImplementation
      {

      public:
        QueueImplementation(const char* inLabel, queue::kind_type inKind)
          : mLabel(inLabel ? strdup(inLabel) : nullptr)
          , mStrand(inKind == queue::SERIAL
            ? Service::sharedInstance().makeStrand()
            : nullptr)
        {
        }

        ~QueueImplementation()
        {
          Service::sharedInstance().destroyStrand(mStrand);

          free((void*)mLabel);
          mLabel = nullptr;
        }

        const char * label() const
        {
          return mLabel;
        }

        template <typename F>
        void call(F&& function)
        {
          Service::sharedInstance().call(mStrand, std::forward<F>(function));
        }

      private:
        boost::asio::io_service::strand* mStrand;
        const char*       mLabel;

      };

      class GroupImplementation
      {
      public:
        GroupImplementation()
          : mCount(0)
        {
        }

        ~GroupImplementation()
        {
        }

        template <typename F>
        void call(queue& queue, F&& function)
        {
          op_begin();
          queue.call([this, function] {
						helper h(*this);
            callAndCatchExceptions(function);
					});
        }

        void wait()
        {
          lock_type lock(mMutex);
          mWorking.wait(lock, [this] () {
            return this->isDone();
          });
        }

        bool wait_for(uint64_t timeout_in_microseconds)
        {
          lock_type lock(mMutex);
          return mWorking.wait_for(lock,
            std::chrono::microseconds(timeout_in_microseconds),
            [this] () {
            return this->isDone();
          });
        }

      private:
        long long               mCount;
        std::mutex              mMutex;
        std::condition_variable mWorking;
        typedef std::unique_lock<std::mutex> lock_type;

        struct helper
        {
          helper(GroupImplementation& inGroup)
            : group(inGroup)
          {
          }

          ~helper()
          {
            group.op_end();
          }

          GroupImplementation& group;
        };

        bool isDone() const
        {
          return this->mCount <= 0;
        }

        void op_begin()
        {
          lock_type lock(mMutex);
          mCount++;
        }

        void op_end()
        {
          lock_type lock(mMutex);
          mCount--;
          if (isDone()) {
            mWorking.notify_one();
          }
        }

      };

      class TimerImplementation
        : public std::enable_shared_from_this<TimerImplementation>
      {

      public:
        TimerImplementation(const queue& queue,
          time::type repeat_interval_in_microseconds,
          const timer::callback_type& callback,
          bool inRepeats = false)
				: mTimer(Service::sharedInstance().io_service())
          , mQueue(queue)
          , mInterval(repeat_interval_in_microseconds)
          , mCallback(callback)
          , mRepeats(inRepeats)
        {
        }

        void handler(const boost::system::error_code& error)
        {
          if (error == boost::asio::error::operation_aborted)
            return;
          if (mRepeats)
            start();
          mQueue.call([this] {
            mCallback(timer(shared_from_this()));
          });
        }

        void start()
        {
					const auto interval = std::max((time::type)0, mInterval);
          mTimer.expires_from_now(boost::posix_time::microseconds(interval));
          mTimer.async_wait([this] (const boost::system::error_code& error) {
            this->handler(error);
          });
        }

        void stop()
        {
          mTimer.cancel();
        }

      private:
        typedef boost::asio::deadline_timer   timer_type;
        timer_type mTimer;
        queue mQueue;
        time::type mInterval;
        timer::callback_type  mCallback;
        bool mRepeats;

      };

    }
  }
}

#endif // defined __jerome_async_boost_hpp__
