cmake_minimum_required(VERSION 3.17)
project(HackDL)
find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_STANDARD 20)

#set(BOOST_ROOT "/usr/local/Cellar/boost/1.75.0_1")
#SET (BOOST_ROOT "/usr/local/Cellar/boost/1.75.0_1/")
#SET (BOOST_INCLUDEDIR "/usr/local/Cellar/boost/1.75.0_1/include")
#SET (BOOST_LIBRARYDIR "/usr/local/Cellar/boost/1.75.0_1/lib")

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)

#find_package( Boost REQUIRED COMPONENTS program_options system thread filesystem serialization REQUIRED )

#include_directories(${Boost_INCLUDE_DIRS})


#find_package(Boost 1.75.0 COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#find_package(Boost COMPONENTS log log_setup thread filesystem system)

#find_package(jsoncpp CONFIG REQUIRED)
#pkg_check_modules(JSONCPP jsoncpp)
#link_libraries(${JSONCPP_LIBRARIES})
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp/lib)
#include_directories("${JSONCPP_INCLUDE_DIRS}")
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp/lib)
add_executable(HackDL hackdl.cpp verilog/Parser.cpp verilog/Parser.h helpers/gui.cpp helpers/gui.h helpers/basic.cpp
        helpers/basic.h verilog/definitions.h verilog/blocks.h verilog/design.cpp verilog/design.h
        jsoncpp/jsoncpp.cpp jsoncpp/json/json.h jsoncpp/json/json-forwards.h helpers/hierarchy.cpp helpers/hierarchy.h
        helpers/flags.h verilog/blocks.cpp verilog/taskers.cpp verilog/taskers.h)
#target_link_libraries(${PROJECT_NAME}
#        ${Boost_LIBRARIES}
#        ${Boost_FILESYSTEM_LIBRARY}
#        ${Boost_SYSTEM_LIBRARY})
#find_package(Boost COMPONENTS system REQUIRED)
#target_link_libraries(HLSpy ${Boost_SYSTEM_LIBRARY})
#target_link_libraries(${PROJECT_NAME} ${JSONCPP_LIBRARIES})
#target_link_libraries(main PRIVATE jsoncpp_object jsoncpp_static)

