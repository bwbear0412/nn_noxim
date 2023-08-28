cmake_minimum_required(VERSION 3.0)
project(noxim)

set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS "-O3 -g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pthread")
# set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-pthread")
# set(CMAKE_MODULE_LINKER_FLAGS "-pthread")
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(
        /usr/systemc-2.3.3/include
)

link_libraries(
        /usr/systemc-2.3.3/lib-linux64/libsystemc-2.3.3.so
)

#add_definitions(-DSC_NO_WRITE_CHECK)

find_package(SystemCLanguage CONFIG REQUIRED)
# set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

aux_source_directory(./src SRC_LST)

add_executable(noxim ${SRC_LST})

target_link_libraries(noxim SystemC::systemc)
# target_link_libraries(noxim systemc.a)