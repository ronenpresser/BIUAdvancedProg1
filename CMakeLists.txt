cmake_minimum_required(VERSION 3.15)
project(BIUAdvancedProg1)

set(CMAKE_CXX_STANDARD 14)

add_executable(BIUAdvancedProg1 Lexer.cpp Lexer.h main.cpp InterpretTool.cpp InterpretTool.h Expression.cpp Expression.h Parser.cpp Parser.h Command.cpp Command.h)
find_package (Threads REQUIRED)

target_link_libraries(BIUAdvancedProg1 ${CMAKE_THREAD_LIBS_INIT})