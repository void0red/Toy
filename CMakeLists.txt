cmake_minimum_required(VERSION 3.30)
project(Toy)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

message(STATUS "llvm: ${LLVM_VERSION}")
message(STATUS "flex: ${FLEX_VERSION}")
message(STATUS "bison: ${BISON_VERSION}")

FLEX_TARGET(ToyLexer
        toy.l
        ${CMAKE_CURRENT_BINARY_DIR}/toy.lex.cpp
)

BISON_TARGET(ToyParser
        toy.y
        ${CMAKE_CURRENT_BINARY_DIR}/toy.tab.cpp
        COMPILE_FLAGS -Wcounterexamples
)

ADD_FLEX_BISON_DEPENDENCY(ToyLexer ToyParser)

add_library(ToyImpl
        AST.cpp
        ${FLEX_ToyLexer_OUTPUTS}
        ${BISON_ToyParser_OUTPUTS}
        Scanner.hpp
)
target_include_directories(ToyImpl PUBLIC
        ${FLEX_INCLUDE_DIRS}
        ${LLVM_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

target_link_directories(ToyImpl PUBLIC
        ${LLVM_LIBRARY_DIRS})


add_executable(Toy main.cpp)

target_link_libraries(Toy ToyImpl LLVM)