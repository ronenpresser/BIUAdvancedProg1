cmake_minimum_required(VERSION 3.15)
project(BIUAdvancedProg1)

set(CMAKE_CXX_STANDARD 14)

add_executable(BIUAdvancedProg1 Lexer.cpp Lexer.h main.cpp ex1.cpp ex1.h Expression.cpp Expression.h Parser.cpp Parser.h Command.cpp Command.h)