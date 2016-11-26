cmake_minimum_required(VERSION 3.6)

project(biblio_test)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(test_SRCS bibliotest.cpp)

add_executable(runUnitTests ${test_SRCS})

target_link_libraries(runUnitTests gtest gtest_main biblio_lib ${CURL_LIBRARIES} poppler-cpp tesseract lept config++ opencv_core opencv_highgui opencv_imgproc)
