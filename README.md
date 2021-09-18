# rabbitmqclient

RabbitMQ client example, base on `AMQP-CPP`.

## Quick Start

Test environment:

- ubuntu 16.04
- cmake 3.17.5
- rabbitmq-server 3.5.7
- erlang 18.3

Build:

```bash
mkdir build && cd build
cmake .. && make
```

## Secure Connect with SSL

Certificate file dependency, make your own certificate file and replace the listed file, the default is:

```bash
build/ssl/ca/cacert.pem
build/ssl/client/rabbitmq_client.cert.pem
build/ssl/client/rabbitmq_client.key.pem
```

Set custom certificate file path options:

```bash
cmake -DAMQPCPP_SSL_CACERTS={your-cacert-file} -DAMQPCPP_SSL_CERTFILE={your-client-cert-file} -DAMQPCPP_SSL_KEYFILE={you-client-key-file} ..
```

Usage:

Using the `amqps://` protocol instead of `amqp://`:

```cpp
// general.
RabbitmqClient::InitParameter("amqp://guest:guest@localhost/", "exchange", "queue", "routekey");
// using SSL.
RabbitmqClient::InitParameter("amqps://guest:guest@localhost/", "exchange", "queue", "routekey");
```

## AMQP-CPP changes

Please see `docs/amqpcpp_ssl_fixed.diff`.

## Reference

- [AMQPCPP](https://github.com/CopernicaMarketingSoftware/AMQP-CPP)
- [examples.amqp-cpp](https://github.com/hoxnox/examples.amqp-cpp)
