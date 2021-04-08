cmake_minimum_required(VERSION 3.10)

project(chess VERSION 1.0)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

add_executable(chess src/chess_board.cpp
                     main.cpp)

target_include_directories(chess PUBLIC include)

target_compile_features(chess PRIVATE cxx_std_17)

target_compile_options(chess
                       PRIVATE
                       $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /TP>
                       $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>)

target_link_libraries(chess sfml-graphics sfml-audio)
