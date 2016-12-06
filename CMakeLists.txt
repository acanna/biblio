cmake_minimum_required(VERSION 3.6)

project(biblio)

set(CURL_USE_STATIC_LIBS OFF)
set(CURL_USE_MULTITHREADED ON)

find_package(CURL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic")

add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)

include_directories(${CURL_INCLUDE_DIRS})

add_subdirectory(test)

set(SOURCE_FILES

        lib/json/json-forwards.h
        lib/json/json.h
        lib/json/value.h
        lib/json/jsoncpp.cpp
        lib/tinydir/tinydir.h

        src/ScienceDirectRequester.cpp
        src/ScienceDirectRequester.h
        src/NatureRequester.cpp
        src/NatureRequester.h
        src/ScopusRequester.cpp
        src/ScopusRequester.h
        src/ArxivRequester.h
        src/ArxivRequester.cpp
        src/ArticleInfo.cpp
        src/ArticleInfo.h
        src/Biblio_exception.h
        src/BiblioManager.cpp
        src/BiblioManager.h
        src/main.cpp
        src/Parser.cpp
        src/Parser.h
        src/PictureParser.cpp
        src/PictureParser.h
        src/tools.cpp
        src/tools.h
        src/DBLPRequester.h
        src/DBLPRequester.cpp
        src/Requester.h
        src/Requester.cpp
        src/SpringerRequester.cpp
        src/SpringerRequester.h)

add_library(biblio_lib ${SOURCE_FILES})
add_executable(biblio ${SOURCE_FILES})
target_link_libraries(biblio gtest gtest_main ${CURL_LIBRARIES} poppler-cpp tesseract lept config++ opencv_core opencv_highgui opencv_imgproc tinyxml2)

