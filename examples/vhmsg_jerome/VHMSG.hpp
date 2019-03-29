//
//  VHMSG.hpp
//
//  Created by Anton Leuski on 9/29/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef VHMSG_hpp
#define VHMSG_hpp

#include <jerome/types.hpp>
#include "VHMSGHandler.hpp"

namespace ict {
  namespace vhmsg {

    using namespace jerome;

    class Model
    {
    public:
      struct DEFAULT
      {
        static constexpr const char* HOST = "127.0.0.1";
        static constexpr const char* PORT = "61616";
        static constexpr const char* SCOPE = "DEFAULT_SCOPE";
      };

      Model(const String& inComponentID,
        const String& inHost = DEFAULT::HOST,
        const String& inPort = DEFAULT::PORT,
        const String& inScope = DEFAULT::SCOPE)
        : mHost(inHost)
        , mPort(inPort)
        , mScope(inScope)
        , mComponentID(inComponentID)
				, mConnected(true)
      {
      }

      String host() const
      {
        return mHost;
      }

      String port() const
      {
        return mPort;
      }

      String scope() const
      {
        return mScope;
      }

      String componentID() const
      {
        return mComponentID;
      }

      bool connected() const
      {
        return mConnected;
      }

      void setConnected(bool inConnected)
      {
        mConnected = inConnected;
      }

    private:
      String mHost;
      String mPort;
      String mScope;
      String mComponentID;
      bool mConnected;
    };

    namespace detail {
      class ClientImpl;
    }

    class Client
      : public ReferenceClassInterface<detail::ClientImpl>
    {
    public:
      typedef ReferenceClassInterface<detail::ClientImpl> parent_type;
			typedef shared_ptr<detail::Handler>	listener_token_type;

      Client(const Model& model);
      OptionalError start();
      void stop();
      const Model& model() const;

      template <typename L>
      listener_token_type setListenerForEvent(L&& listener,
                               const String& inVHMSGEventHead,
                               const String& inCommand = "")
      {
        return setListener(detail::Handler::newHandler(inVHMSGEventHead, inCommand,
            std::forward<L>(listener)));
      }

			void removeListener(const listener_token_type& token);

			template <typename ...Args>
      void send(const std::string& in_head, Args&& ... args)
			{
				doSend(in_head, detail::foldAsString(std::forward<Args>(args)...));
			}

    private:
      void doSend(const std::string& in_head, const std::string& in_message);
      listener_token_type setListener(const shared_ptr<detail::Handler>& h);
    };

  }
}

#endif /* VHMSG_hpp */
