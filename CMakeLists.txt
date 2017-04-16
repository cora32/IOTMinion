cmake_minimum_required(VERSION 3.6)
project(IOTMinion)

set(SOURCE_FILES main.cpp randomizer/IpListParser.cpp randomizer/IpListParser.h netcon/NetCon.cpp netcon/NetCon.h constants.h Logger/Logger.cpp Logger/Logger.h)

#if(CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "-O3")        ## Optimize
#    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
#endif ()

set(Boost_ADDITIONAL_VERSIONS "1.63.0" "1.63" ${Boost_ADDITIONAL_VERSIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#add_compile_options(-std=c++11)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
#set(Boost_COMPILER              "-vc140")
#set(Boost_COMPILER              "-mgw62")
#set(Boost_DEBUG ON)

unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
unset(Boost_LIBRARIES CACHE)

find_package(Boost REQUIRED system filesystem thread program_options)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_executable(IOTMinion ${SOURCE_FILES})
    IF (WIN32)
        target_link_libraries(IOTMinion ${Boost_LIBRARIES} ws2_32)
    ELSE ()
        target_link_libraries(IOTMinion ${Boost_LIBRARIES})
    ENDIF ()
endif ()
set(CMAKE_BUILD_TYPE Debug)