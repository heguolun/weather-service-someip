project(weather-app)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-g -std=c++14")

find_package(vsomeip3 REQUIRED)
find_package(Boost COMPONENTS thread log system REQUIRED)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include" ,
    ${Boost_INCLUDE_DIR}
)

add_executable(
    mainclient
    ../src/mainclient.cpp
)
target_link_libraries(mainclient vsomeip3 ${Boost_LIBRARIES})

add_executable(
    codeprovider
    ../src/codeprovider.cpp
)
target_link_libraries(codeprovider vsomeip3 ${Boost_LIBRARIES})

add_executable(
    reportprovider
    ../src/reportprovider.cpp
)
target_link_libraries(reportprovider vsomeip3 ${Boost_LIBRARIES})