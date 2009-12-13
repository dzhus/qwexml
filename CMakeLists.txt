cmake_minimum_required(VERSION 2.8)

PROJECT(qwexml)

ADD_LIBRARY(qwexml SHARED qwexml.hpp)
SET_TARGET_PROPERTIES(qwexml PROPERTIES
  LINKER_LANGUAGE CXX)

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

ADD_DEFINITIONS(-DQWE_USE_STL)
ADD_EXECUTABLE(qwetest qwetest.cpp)
ADD_EXECUTABLE(qweparse qweparse.cpp)

TARGET_LINK_LIBRARIES(qwetest qwexml)
TARGET_LINK_LIBRARIES(qweparse qwexml)

ADD_TEST(NAME qwetest
  COMMAND qwetest)
ADD_TEST(NAME qweparse
  COMMAND qweparse)
ENABLE_TESTING()

ADD_CUSTOM_TARGET(doc doxygen Doxyfile)
ADD_CUSTOM_TARGET(hgclean rm -frv `hg status --unknown --no-status`)
