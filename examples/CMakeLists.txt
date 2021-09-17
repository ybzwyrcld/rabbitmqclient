cmake_minimum_required(VERSION 3.0)

add_executable(rabbitmq_sender
  rabbitmq_sender.cc
)
target_link_libraries(rabbitmq_sender PRIVATE
  rabbitmqclient
)

add_executable(rabbitmq_receiver
  rabbitmq_receiver.cc
)
target_link_libraries(rabbitmq_receiver PRIVATE
  rabbitmqclient
)

