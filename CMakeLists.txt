cmake_minimum_required(VERSION 3.0)
project(paddle-mobile)
add_definitions(-DPADDLE_MOBILE_DEBUG)
add_definitions(-DENABLE_EXCEPTION)

if(IS_MAC)
    add_definitions(-DX86)
elseif(IS_IOS)
    add_definitions(-DIOS)
elseif(V7)
    add_definitions(-DARMV7)
elseif(V8)
    add_definitions(-DARMV8)
else ()
    add_definitions(-DX86)
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

file(GLOB_RECURSE PADDLE_MOBILE_CC src/*.cc src/*.cpp src/*.c)
file(GLOB_RECURSE PADDLE_MOBILE_H src/*.h)

include_directories(src/)

option(USE_OPENMP "openmp support" OFF)
if(USE_OPENMP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    add_definitions(-DPADDLE_MOBILE_USE_OPENMP)
endif()

if (googlenet)
    add_definitions(-DCONCAT_OP)
    add_definitions(-DCONV_OP)
    add_definitions(-DLRN_OP)
    add_definitions(-DMUL_OP)
    add_definitions(-DELEMENTWISEADD_OP)
    add_definitions(-DFUSION_FC_OP)
    add_definitions(-DPOOL_OP)
    add_definitions(-DRELU_OP)
elseif (mobilenet)
    add_definitions(-DCONV_OP)
    add_definitions(-DELEMENTWISEADD_OP)
    add_definitions(-DRELU_OP)
    add_definitions(-DSOFTMAX_OP)
    add_definitions(-DSOFTMAX_OP)
    add_definitions(-DDEPTHWISECONV_OP)
    add_definitions(-DBATCHNORM_OP)
    add_definitions(-DPOOL_OP)
    add_definitions(-DRESHAPE_OP)
elseif (yolo)
    add_definitions(-DBATCHNORM_OP)
    add_definitions(-DCONV_OP)
    add_definitions(-DRELU_OP)
    add_definitions(-DELEMENTWISEADD_OP)
elseif (squeezenet)
    add_definitions(-DCONCAT_OP)
    add_definitions(-DCONV_OP)
    add_definitions(-DRELU_OP)
    add_definitions(-DELEMENTWISEADD_OP)
    add_definitions(-DPOOL_OP)
    add_definitions(-DRESHAPE_OP)
    add_definitions(-DSOFTMAX_OP)
elseif(resnet)
    add_definitions(-DCONV_OP)
    add_definitions(-DBATCHNORM_OP)
    add_definitions(-DELEMENTWISEADD_OP)
    add_definitions(-DSOFTMAX_OP)
    add_definitions(-DMUL_OP)
    add_definitions(-DPOOL_OP)
    add_definitions(-DRELU_OP)
else ()
    add_definitions(-DBATCHNORM_OP)
    add_definitions(-DBOXCODER_OP)
    add_definitions(-DCONCAT_OP)
    add_definitions(-DCONV_OP)
    add_definitions(-DDEPTHWISECONV_OP)
    add_definitions(-DELEMENTWISEADD_OP)
    add_definitions(-DFUSIONCONVADD_OP)
    add_definitions(-DCONVADDRELU_OP)
    add_definitions(-DFUSION_FC_OP)
    add_definitions(-DLRN_OP)
    add_definitions(-DMUL_OP)
    add_definitions(-DMULTICLASSNMS_OP)
    add_definitions(-DPOOL_OP)
    add_definitions(-DPRIORBOX_OP)
    add_definitions(-DRELU_OP)
    add_definitions(-DRESHAPE_OP)
    add_definitions(-DSIGMOID_OP)
    add_definitions(-DSOFTMAX_OP)
    add_definitions(-DTRANSPOSE_OP)
endif()

add_library(paddle-mobile SHARED ${PADDLE_MOBILE_CC} ${PADDLE_MOBILE_H})

add_subdirectory(test)
