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

// @File    :  rabbitmq_client.cc
// @Version :  1.0
// @Time    :  2021/09/16 11:06:03
// @Author  :  Meng Yuming
// @Contact :  mengyuming@hotmail.com
// @Desc    :  None

#include "rabbitmq_client/rabbitmq_client.h"

#include <stdint.h>

#include <thread>  // NOLINT

#include <event2/event.h>
#include <amqpcpp.h>
#include <amqpcpp/libevent.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

namespace rabbitmqclient {

class MyHandler : public AMQP::LibEventHandler {
 public:
  explicit MyHandler(struct event_base *evbase) :
      AMQP::LibEventHandler(evbase), evbase_(evbase) { }
  MyHandler(const MyHandler&) = delete;
  MyHandler& operator=(const MyHandler&) = delete;
  virtual ~MyHandler() = default;

 private:
  void onError(AMQP::TcpConnection *connection,
      const char *message) override {
    std::cout << "error: " << message << std::endl;
    event_base_loopbreak(evbase_);
  }
  struct event_base *evbase_ = nullptr;
};

RabbitmqClient::RabbitmqClient() {
  service_is_running_.store(false);
  connection_ptr_ = nullptr;
}

RabbitmqClient::~RabbitmqClient() {
  ConsumerStop();
  if (!messages_.empty()) {
    messages_.clear();
  }
}

int RabbitmqClient::SendMessage(char const* const message, int size) {
  int ret = 0;
  auto evbase = event_base_new();
  MyHandler handler(evbase);
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_library_init();
#else
  OPENSSL_init_ssl(0, NULL);
#endif
  AMQP::Address address(url_);
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);
  channel.onError([&evbase, &ret](const char *err_message) {
      std::cout << "Channel error: " << err_message << std::endl;
      event_base_loopbreak(evbase);
      ret = -1; });
  channel.declareExchange(exchange_, AMQP::topic)
      .onError([&] (const char* msg) -> void {
          std::cout << "ERROR: " << msg << std::endl;
          event_base_loopbreak(evbase);
       })
      .onSuccess([&] (void) -> void {
          channel.publish(exchange_, routekey_,
              message, size, 0);
          connection.close();
       });
  // Run loop.
  event_base_dispatch(evbase);
  event_base_free(evbase);
  return ret;
}

void RabbitmqClient::ConsumerRun(void) {
  if (service_is_running_.load()) return;
  thread_.reset(&RabbitmqClient::ConsumerService, this);
}

void RabbitmqClient::ConsumerStop(void) {
  service_is_running_.store(false);
  if (connection_ptr_ != nullptr) {
    reinterpret_cast<AMQP::TcpConnection *>(connection_ptr_)->close();
    connection_ptr_ = nullptr;
  }
  thread_.join();
}

void RabbitmqClient::ConsumerService(void) {
  service_is_running_.store(true);
  auto evbase = event_base_new();
  MyHandler handler(evbase);
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_library_init();
#else
  OPENSSL_init_ssl(0, NULL);
#endif
  AMQP::Address address(url_);
  AMQP::TcpConnection connection(&handler, address);
  AMQP::TcpChannel channel(&connection);
  channel.onError([&evbase] (const char *err_message) -> void {
      std::cout << "Channel error: " << err_message << std::endl;
      event_base_loopbreak(evbase);
  });
  channel.declareExchange(exchange_, AMQP::topic);
  channel.declareQueue(queue_, AMQP::exclusive);
  channel.bindQueue(exchange_, queue_, routekey_);
  channel.consume(queue_, AMQP::noack)
      .onReceived([&] (const AMQP::Message& msg, uint64_t, bool) -> void {
          if (msg.bodySize() <= 0) return;
          messages_.push_back({msg.body(), msg.body()+msg.bodySize()});
      });
  connection_ptr_ = &connection;
  printf("Rabbitmq listen service running...\n");
  // Run loop.
  event_base_dispatch(evbase);
  printf("Rabbitmq listen service done.\n");
  event_base_free(evbase);
  // Disconnect.
  if (connection_ptr_ != nullptr) {
    connection.close();
    connection_ptr_ = nullptr;
  }
  service_is_running_.store(false);
}

}  // namespace rabbitmqclient
