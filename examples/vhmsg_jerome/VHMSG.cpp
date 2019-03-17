//
//  VHMSG.cpp
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

#include <functional>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#include <vhmsg.h>
#pragma clang diagnostic pop

#include "VHMSG.hpp"

namespace ict {
  namespace vhmsg {

    namespace detail {

      // -----------------------------------------------------------------------------
      // remove the first word token from the string and return it.
      // Note: we modify the string.
      std::string popToken(std::string& io_string)
      {
        std::string::size_type pos;
        const String spaces(" \t\n\r");

        // skip to the first non space character
        pos = io_string.find_first_not_of(spaces);
        if (pos == std::string::npos) {
          // None exist. Clear the string. Return empty string
          io_string = "";
          return "";
        }

        if (pos != 0) {
          io_string = io_string.substr(pos);
        }

        // we start with non-space character
        // find the first space character
        pos = io_string.find_first_of(spaces);

        if (pos == std::string::npos) {
          // None exist. Return the whole string
          std::string result = io_string;
          io_string = "";
          return result;
        }

        // break the string
        std::string result = io_string.substr(0, pos);
        io_string = io_string.substr(pos);

        // skip to the first non-space character in the reminder
        pos = io_string.find_first_not_of(spaces);
        if (pos == std::string::npos) {
          io_string = "";
        } else if (pos != 0) {
          io_string = io_string.substr(pos);
        }

        return result;
      }

      class ClientImpl
        : public::vhmsg::Listener
      {
      public:
        ClientImpl(const Model& model)
          : mModel(model)
        {
        }

        virtual ~ClientImpl()
        {
          disconnect();
        }

        const Model& model() const
        {
          return mModel;
        }

        OptionalError start()
        {
          OptionalError error = Error::NO_ERROR;
          if (model().connected()) {
            error = connect();
            mModel.setConnected(mClient.IsOpen());
          }
          return error;
        }

        void stop()
        {
          disconnect();
        }

        void OnMessage(const::vhmsg::Message& in_message) override
        {
          onMessage(in_message.GetString());
        }

        void send(const std::string& in_head, const std::string& in_message)
        {
          if (!mClient.IsOpen()) return;
          mClient.Send(in_head.c_str(), in_message.c_str());
        }

				Client::listener_token_type setListener(const shared_ptr<detail::Handler>& h)
				{
					auto heads = subscriptions();
					mHandlers.push_back(h);
					if (heads.find(h->head()) == heads.end()) {
						if (mClient.IsOpen())
							mClient.Subscribe(h->head().c_str());
					}
					return h;
				}
				
				void removeListener(const Client::listener_token_type& h)
				{
					std::remove(mHandlers.begin(), mHandlers.end(), h);
					auto heads = subscriptions();
					if (heads.find(h->head()) == heads.end()) {
						if (mClient.IsOpen())
							mClient.Unsubscribe(h->head().c_str());
					}
				}
				
      private:
        void onMessage(std::string in_message)
        {
          String head = popToken(in_message);
          for (auto& h : mHandlers) {
            if (h->head() != head) continue;
            if (h->command() != "") {
              String message = in_message;
              String command = popToken(message);
              if (h->command() != command) continue;
              h->operator () (message);
            } else {
              h->operator () (in_message);
            }
          }
        }

        OptionalError connect()
        {
          if (mClient.IsOpen()) return Error::NO_ERROR;

          mClient.SetScope(model().scope().c_str());

          if (!mClient.OpenConnection(model().host().c_str(),
                model().port().c_str())) {

            return Error("Failed to connect to VHMsg server."
                        , "Please check that the server is running at " +
              model().host() + ":" + model().port());
          }

          mClient.SetListener(this);

          for (const auto& s : subscriptions()) {
            mClient.Subscribe(s.c_str());
          }

          send("vrComponent", model().componentID());

          return Error::NO_ERROR;
        }

        void  disconnect()
        {
          if (!mClient.IsOpen()) return;

          send("vrProcEnd", model().componentID());

          for (const auto& s : subscriptions()) {
            mClient.Unsubscribe(s.c_str());
          }

          mClient.CloseConnection();
        }

        std::unordered_set<String> subscriptions() const
        {
          std::unordered_set<String>  result;
          for (const auto& h : mHandlers) {
            result.insert(h->head());
          }
          return result;
        }

        Model mModel;
        ::vhmsg::Client mClient;
        std::vector<shared_ptr<Handler>> mHandlers;
      };

    }

    Client::Client(const Model& model)
      : parent_type(std::make_shared<detail::ClientImpl>(model))
    {
      setListenerForEvent([this] () {
        this->send("vrComponent", this->model().componentID());
      }, "vrAllCall");
    }

    OptionalError Client::start()
    {
      return implementation().start();
    }

    void Client::stop()
    {
      implementation().stop();
    }

    const Model& Client::model() const
    {
      return implementation().model();
    }

    void Client::doSend(const std::string& in_head, const std::string& in_message)
    {
      implementation().send(in_head, in_message);
    }
		
		Client::listener_token_type Client::setListener(const shared_ptr<detail::Handler>& h)
		{
			return implementation().setListener(h);
		}

		void Client::removeListener(const listener_token_type &token)
		{
			implementation().removeListener(token);
		}
  }
}
