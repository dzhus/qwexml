PROJECT(qwexml)
ADD_LIBRARY(qwexml STATIC qwexml.cpp)
ADD_EXECUTABLE(test qwetest.cpp)
TARGET_LINK_LIBRARIES(test qwexml qwelist)
