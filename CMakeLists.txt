cmake_minimum_required(VERSION 3.6)
project(Bowser)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

find_package(ZLIB REQUIRED)

include_directories(
	include/
    external/)

add_executable(Bowser src/main.cpp
					  src/connection.cpp
                      src/url.cpp
                      src/log.cpp
                      src/http_response.cpp
                      src/http_request.cpp
                      src/html_parser.cpp)

target_link_libraries(Bowser OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)
