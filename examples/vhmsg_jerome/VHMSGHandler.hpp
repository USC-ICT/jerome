//
//  VHMSGHandler.hpp
//
//  Created by Anton Leuski on 9/29/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef VHMSGHandler_h
#define VHMSGHandler_h

#include <array>
#include <sstream>

#include <jerome/types.hpp>
#include <jerome/type/FunctionSignature.hpp>

namespace ict {
  namespace vhmsg {
    namespace detail {
      using namespace jerome;

      std::string popToken(std::string& io_string);

      class Handler
      {
      public:
        Handler(const String& inName, const String& inCommand)
          : mEvent(inName)
          , mCommand(inCommand)
        {
        }

        virtual ~Handler()
        {
        }

        virtual void operator () (const String& inMessage) = 0;

        const String& head() const
        {
          return mEvent;
        }

        const String& command() const
        {
          return mCommand;
        }

        template <typename F>
        static typename std::enable_if<is_callable<F>::value,
          shared_ptr<Handler>>::type newHandler(const String& inName,
                                                const String& inCommand,
                                                F&& inF);

      private:
        String  mEvent;
        String  mCommand;
      };

      template <typename T>
      class from_String
      {
      public:
        static T convert(const String& s)
        {
          if (s.length() == 0) return T();
          std::istringstream str(s);
          T v;
          str >> v;
          return v;
        }

      };

      template <>
      class from_String<String>
      {
      public:
        static String convert(const String& s)
        {
          return s;
        }
      };

      template <typename>
      class HandlerTemplate;

      template <typename ... Args>
      class HandlerTemplate<void(Args ...)>
        : public Handler
      {
      public:
        HandlerTemplate(const String& inName,
          const String& inCommand,
          const std::function<void(Args ...)>& inF)
          : Handler(inName, inCommand)
          , mHandler(inF)
        {
        }

        void operator () (const String& inMessage) override
        {
          std::array<String, sizeof ... (Args)> stringArguments;
          String message = inMessage;
          if (stringArguments.size() > 0) {
            for (std::size_t i = 0; i < stringArguments.size() - 1; ++i)
              stringArguments[i] = popToken(message);
            stringArguments[stringArguments.size() - 1] = message;
          }

          call(stringArguments, index_sequence_for<Args ...> {});
        }

      private:
        std::function<void(Args ...)>  mHandler;

        template <std::size_t ... Indices>
        void call(std::array<String, sizeof ... (Args)>& callbackArguments,
                  index_sequence<Indices ...> )
        {
          mHandler(from_String<typename std::decay<Args>::type>::convert(
              callbackArguments[Indices]) ...);
        }

      };

      template <typename F>
      typename std::enable_if<is_callable<F>::value,
        shared_ptr<Handler>>::type Handler::newHandler(const String& inName,
                                                       const String& inCommand,
                                                       F&& inF)
      {
        typedef typename function_signature<F>::type  sig;
        return std::make_shared<HandlerTemplate<sig>>(inName,
          inCommand,
          std::function<sig>(std::forward<F>(inF)));
      }

			template <typename T>
			inline String convert_to_string(const T& v)
			{
				return std::to_string(v);
			}

			inline String convert_to_string(const String& v)
			{
				return v;
			}

			inline String convert_to_string(const char* v)
			{
				return String(v);
			}

      template <typename ... Args>
      inline String foldAsString(Args&& ... args)
      {
        std::array<String, sizeof ... (Args)> strs { convert_to_string(
                                                       args) ... };
				String result;
        for (std::size_t i = 0; i < strs.size(); ++i) {
					if (i > 0)
						result += " ";
          result += strs[i];
				}
        return result;
      }

    }
  }
}

#endif /* VHMSGHandler_h */