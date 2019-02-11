cmake_minimum_required(VERSION 3.12)
project(HuffmanArchiver C)

set(CMAKE_C_STANDARD 11)

add_executable(HuffmanArchiver main.c definitions.h pqueue.c pqueue.h)