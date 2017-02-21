cmake_minimum_required(VERSION 2.8.12.2)
project(ProtocolAnalyser)

set(Boost_INCLUDE_DIR /home/rob/Downloads/boost_1_63_0)
set(Boost_LIBRARY_DIR /home/rob/Downloads/boost_1_63_0/stage/lib)
#find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(include ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(include)
set(SOURCES src/main.cpp
            src/KMeans.cpp 
            src/PacketAnalyser.cpp
            src/CUdpPair.cpp
            src/CUdpProtocolLearner.cpp
            src/CUdpSender.cpp
            src/CUdpReceiver.cpp
            src/KMeansTests.cpp
            src/CUdpProtocolLearnerTests.cpp)

add_compile_options(-std=c++11)

add_executable(tests ${SOURCES})
TARGET_LINK_LIBRARIES(tests pcap pthread boost_system boost_thread)
