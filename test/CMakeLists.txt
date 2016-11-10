cmake_minimum_required(VERSION 3.6)

project(biblio_test)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(test_SRCS bibliotest.cpp)

add_executable(runUnitTests ${test_SRCS})

target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests biblio)

#set(REPO ~/ClionProjects/.repo)

#project(Test)

#project(Example)

#include(CTest)
enable_testing()

#set(gtest_disable_pthreads on) #needed in MinGW
#include(${REPO}/DownloadProject/DownloadProject.cmake)
#download_project(
 #       PROJ                googletest
  #      GIT_REPOSITORY      https://github.com/google/googletest.git
  #      GIT_TAG             master
  #      UPDATE_DISCONNECTED 1
#)

#add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

#set(test_SRCS
 #       DBLP_unittest.cpp
 #       Picture_Parser_title_unittest.cpp
 #       Simple_Parser_title_unittest.cpp)
#add_executable(runUnitTests ${test_SRCS})
#target_link_libraries(runUnitTests gtest gmock core)
add_test(runUnitTests runUnitTests) #included in all tutorials but I don't know what it actually does.