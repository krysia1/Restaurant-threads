cmake_minimum_required(VERSION 3.13)
project(SO2P)

set(CMAKE_CXX_STANDARD 11)

add_executable(SO2P main.cpp Supplier.cpp Supplier.h Waiter.cpp Waiter.h Cook.cpp Cook.h Barman.cpp Barman.h Client.cpp Client.h Toilet.cpp Toilet.h Glass.h)