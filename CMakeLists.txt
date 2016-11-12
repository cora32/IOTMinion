cmake_minimum_required(VERSION 3.6)
project(IOTMinion)

set(SOURCE_FILES main.cpp randomizer/IpListParser.cpp randomizer/IpListParser.h netcon/NetCon.cpp netcon/NetCon.h)

#if(CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "-O3")        ## Optimize
#    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
#endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#add_compile_options(-std=c++11)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
unset(Boost_LIBRARIES CACHE)

find_package(Boost REQUIRED COMPONENTS system filesystem thread)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(IOTMinion ${SOURCE_FILES})
    IF (WIN32)
        target_link_libraries(IOTMinion ${Boost_LIBRARIES} ws2_32)
    ELSE ()
        target_link_libraries(IOTMinion ${Boost_LIBRARIES})
    ENDIF ()
endif ()
set(CMAKE_BUILD_TYPE Debug)