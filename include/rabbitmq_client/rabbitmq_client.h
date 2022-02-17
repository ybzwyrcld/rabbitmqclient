// MIT License
//
// Copyright (c) 2021 Yuming Meng
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @File    :  rabbitmq_client.h
// @Version :  1.0
// @Time    :  2021/09/16 10:49:06
// @Author  :  Meng Yuming
// @Contact :  mengyuming@hotmail.com
// @Desc    :  None

#ifndef RABBITMQCLIENT_RABBITMQ_CLIENT_H_
#define RABBITMQCLIENT_RABBITMQ_CLIENT_H_

#include <atomic>
#include <string>
#include <list>
#include <vector>

#include "../thread_raii.h"


namespace rabbitmqclient {

// URL example:
//   'amqp://guest:guest@localhost/'
//   'amqp://guest:guest@localhost/vhost'
//   'amqps://guest:guest@localhost/'
//   'amqps://guest:guest@localhost/vhost'
// Description:
//   @amqp           general tcp connection type (port 5672);
//   @amqps          secure connection with SSL/TLS (port 5671);
//   @guest:guest    username:password;
//   @localhost      hostname or ip;
//   @vhost          virtual host, optional.

//
// For producer, parameter 'queue' is not used.
//
class RabbitmqClient {
 public:
  RabbitmqClient();
  RabbitmqClient(std::string const& url, std::string const& exchange,
      std::string const& queue, std::string const& routekey) :
          url_(url), exchange_(exchange), queue_(queue), routekey_(routekey) {
    service_is_running_.store(false);
    connection_ptr_ = nullptr;
  }
  // RabbitmqClient is neither copyable nor movable.
  RabbitmqClient(RabbitmqClient const&) = delete;
  RabbitmqClient(RabbitmqClient&&) = delete;
  RabbitmqClient& operator=(RabbitmqClient const&) = delete;
  RabbitmqClient& operator=(RabbitmqClient&&) = delete;
  ~RabbitmqClient();

  void InitParameter(std::string const& url, std::string const& exchange,
      std::string const& queue, std::string const& routekey) {
    url_ = url;
    exchange_ = exchange;
    queue_ = queue;
    routekey_ = routekey;
  }
  //
  // For producer.
  //
  // Generic method of sending message.
  int SendMessage(char const* const message, int size);
  int SendMessage(const std::vector<char> &message) {
    return SendMessage(&message.front(), message.size());
  }
  // Using custom 'exchange' and 'routekey'.
  int SendMessage(std::string const& exchange, std::string const& routekey,
      char const* const message, int size) {
    exchange_ = exchange;
    routekey_ = routekey;
    return SendMessage(message, size);
  }
  int SendMessage(std::string const& exchange, std::string const& routekey,
      std::vector<char> const& message) {
    return SendMessage(exchange, routekey, &message.front(), message.size());
  }
  //
  // For consumer.
  //
  // Current message count.
  int get_message_count(void) const {
    return messages_.size();
  }
  // Get the first message in queue.
  int get_message(std::vector<char>* const msg) {
    if (messages_.empty() || msg == nullptr) return -1;
    msg->assign(messages_.front().begin(), messages_.front().end());
    messages_.pop_front();
    return 0;
  }
  bool service_is_running(void) const {
    return service_is_running_.load();
  }
  void ConsumerRun(void);
  void ConsumerStop(void);

 private:
  void ConsumerService(void);

  std::atomic_bool service_is_running_;
  // RabbitMQ parameters.
  std::string url_;
  std::string exchange_;
  std::string queue_;
  std::string routekey_;
  Thread thread_;
  // Connection control.
  void* connection_ptr_;
  // Consumer receive message list.
  std::list<std::vector<char>> messages_;
};

}  // namespace rabbitmqclient

#endif  // RABBITMQCLIENT_RABBITMQ_CLIENT_H_
