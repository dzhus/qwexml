cmake_minimum_required(VERSION 2.8)

PROJECT(qwexml)

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -Wall -pedantic")
SET(CMAKE_BUILD_TYPE "Debug")

ADD_DEFINITIONS(-DQWE_USE_STL)

ADD_LIBRARY(qwexml SHARED qwexml.cpp)
ADD_LIBRARY(qweparse SHARED qweparse.cpp)
ADD_LIBRARY(qwestring SHARED qwestring.cpp)

ADD_EXECUTABLE(qwetest qwetest.cpp)
ADD_EXECUTABLE(qweparsetest qweparsetest.cpp)

TARGET_LINK_LIBRARIES(qwetest qwexml qwestring)
TARGET_LINK_LIBRARIES(qweparse qwexml qwestring)
TARGET_LINK_LIBRARIES(qweparsetest qweparse)

ADD_TEST(NAME internals
  COMMAND qwetest)
ADD_TEST(NAME parsing
  COMMAND parsing-test.py)
ENABLE_TESTING()

ADD_CUSTOM_TARGET(doc doxygen Doxyfile)
ADD_CUSTOM_TARGET(hgclean rm -frv `hg status --unknown --no-status`)
