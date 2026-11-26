# Testes no computador dos componentes C puro (dsp, detector, proto)
#   cmake -S test_host -B test_host/build
#   cmake --build test_host/build
#   ctest --test-dir test_host/build --output-on-failure
cmake_minimum_required(VERSION 3.16)
project(mu_test_host C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(COMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../components)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wdouble-promotion)
endif()

add_library(mu_dsp STATIC ${COMP_DIR}/dsp/dsp.c)
target_include_directories(mu_dsp PUBLIC ${COMP_DIR}/dsp/include)

add_library(mu_detector STATIC ${COMP_DIR}/detector/detector.c ${COMP_DIR}/detector/mu_fsm.c)
target_include_directories(mu_detector PUBLIC ${COMP_DIR}/detector/include)

add_library(mu_proto STATIC ${COMP_DIR}/proto/proto.c)
target_include_directories(mu_proto PUBLIC ${COMP_DIR}/proto/include)

if(NOT MSVC)
    target_link_libraries(mu_dsp PUBLIC m)
endif()

enable_testing()

foreach(t dsp detector proto)
    add_executable(test_${t} test_${t}.c)
    target_include_directories(test_${t} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(test_${t} PRIVATE mu_${t})
    add_test(NAME ${t} COMMAND test_${t})
endforeach()
