##################################################
############# AI WATCH A1 CMAKE ##################
##################################################
## Authors: Denny Caruso and Francesco Pollasto 
## 2022 - 2023

## CMAKE ##

cmake_minimum_required(VERSION 3.4)
set(Boost_USE_STATIC_LIBS OFF)
set(CMAKE_INSTALL_PREFIX "/usr/local/AI_Watch_A1") # if you want to make install it (not recommended)

### Python env
set(Python_VIRTUALENV FIRST)

### Enable C++11
add_definitions(-std=c++11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(AI_Watch_A1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

## PACKAGES ##

##### VARIOUS #####
find_package(Boost COMPONENTS filesystem system thread iostreams chrono date_time regex REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Protobuf REQUIRED)

##### PYTHON #####
find_package(Python 3.9 COMPONENTS Interpreter Development EXACT)
find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)

##### REALSENSE #####
find_package(realsense2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

##### OPENCV #####
find_package(OpenCV REQUIRED)

##### OPENPOSE #####
find_package(GFlags)
find_package(Glog REQUIRED)
find_package(OpenPose REQUIRED)

##### KAFKA #####
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

##### VARIOUS #####
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

## LINKER FLAGS ##

set(CMAKE_EXE_LINKER_FLAGS "-lgflags -ljsoncpp -lboost_system -lglog")

## HEADER FILES ##

### Dependencies
include_directories(${PROTOBUF_INCLUDE_DIR}) # realsense, openpose
include_directories( ${OpenCV_INCLUDE_DIRS} ) # AI Watch A1
include_directories( ${GLIB_INCLUDE_DIRS} ) # kafka, openpose
include_directories( ${LIBRDKAFKA_INCLUDE_DIRS}) # kafka
include_directories( ${BOOST_INCLUDEDIR}) # kafka, openpose
include_directories(${OpenPose_INCLUDE_DIRS} ${CAFFE_INCLUDE_DIRS} ${GFLAGS_INCLUDE_DIR} ${GLOG_INCLUDE_DIR}) # openpose
include_directories(${PYTHON_INCLUDE_DIRS}) # openpose for python

### Main program
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Camera")

## DIRECTORY LINKING ##

link_directories(${BOOST_LIBRARYDIR}) # kafka, openpose

pkg_check_modules(JSONCPP jsoncpp)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

## LIBRARIES ##

### Main library (no category)
add_library(aiwatchmain STATIC main.cpp Points/BodyKeyPoint.cpp Points/Point.cpp Points/Point3D.cpp Output/Skeleton.cpp)

### Room
add_library(aiwatchroom STATIC Room/Room.cpp Room/CVPR_Lab.cpp Room/CoordinateMapping/CoordinateMappingManager.cpp Room/CoordinateMapping/UnityCoordinateMappingManager.cpp )

### Realsense
add_library(aiwatchrs Camera/DepthCamera.cpp Camera/RealSense/RealsenseCamera.cpp Camera/RealSense/RealsenseNetCamera.cpp)

### Blockchain 
add_library(aiwatchblockchain Blockchain/BlockchainLoader.cpp)

### Kafka
add_library(aiwatchkafka Kafka/KafkaManager.cpp)

### Utils 
#add_library(aiwatchutils BlockchainInteraction/BlockchainLoader.cpp BlockchainInteraction/Decorator.cpp BlockchainInteraction/Component.cpp BlockchainInteraction/BlockchainLoaderDecorator.cpp)

### System interface
add_library(aiwatchsys SystemInterface/CLInput.cpp SystemInterface/FileDeleter.cpp)


add_executable(AI_Watch_A1 main.cpp Output/Skeleton.cpp Points/BodyKeyPoint.cpp Points/Point.cpp Points/Point3D.cpp Room/CoordinateMapping/UnityCoordinateMappingManager.cpp Room/CoordinateMapping/CoordinateMappingManager.cpp
                Room/CVPR_Lab.cpp Room/Room.cpp Camera/DepthCamera.cpp Camera/RealSense/RealsenseCamera.cpp Camera/RealSense/RealsenseNetCamera.cpp Output/Skeleton.cpp Kafka/KafkaManager.cpp Blockchain/BlockchainLoader.cpp)

## LINKING ##

target_link_libraries(AI_Watch_A1 PkgConfig::deps glfw OpenGL::GL ${PROTOBUF_LIBRARY} ${BOOST_LIBS} Boost::filesystem ${GLIB_LIBRARIES}) # MULTIPLE DEPENDANTS

### Realsense
target_link_libraries(AI_Watch_A1 ${realsense2_LIBRARY} realsense2-net)
### OpenCV
target_link_libraries(AI_Watch_A1 ${OpenCV_LIBS})
### Kafka
target_link_libraries(AI_Watch_A1 rdkafka++ librdkafka.a librdkafka++.a ZLIB::ZLIB libzstd.a liblz4.a OpenSSL::SSL sasl2)
### JsonCPP
link_libraries(${JSONCPP_LIBRARIES})
target_link_libraries(AI_Watch_A1 ${JSONCPP_LIBRARIES})
### Python BINDING
target_link_libraries(AI_Watch_A1 ${PYTHON_LIBRARIES} pybind11::embed pybind11::module pybind11::pybind11)
### OpenPose
target_link_libraries(AI_Watch_A1 ${CAFFE_LIBRARIES} ${OpenPose_LIBS} gflags ${GFLAGS_LIBRARY} ${GLOG_LIBRARY})

## Blockchain, pybind11 syspath:

target_compile_definitions(aiwatchmain PRIVATE -DCUSTOM_SYS_PATH="null")
target_compile_definitions(AI_Watch_A1 PRIVATE -DCUSTOM_SYS_PATH="null")

## BINARIES AND LIBRARIES
install(TARGETS AI_Watch_A1 DESTINATION)
install(TARGETS aiwatchmain LIBRARY DESTINATION)
install(TARGETS aiwatchroom LIBRARY DESTINATION)
install(TARGETS aiwatchblockchain LIBRARY DESTINATION)
install(TARGETS aiwatchsys LIBRARY DESTINATION)
install(TARGETS aiwatchkafka LIBRARY DESTINATION)
install(TARGETS aiwatchrs LIBRARY DESTINATION)

#install(TARGETS aiwatchutils LIBRARY DESTINATION)


## COPY FILES
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/conf.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/configuration_file.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/blockchain_code/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/blockchain_code/)


## SOURCE HEADER FILES
install(FILES Camera/RealSense/RSFrame.hpp Camera/DepthCamera.hpp Camera/RealSense/RealsenseCamera.hpp Camera/RealSense/RealsenseNetCamera.hpp DESTINATION include)
install(FILES ImageGen/OpenCVImgGen.hpp ImageGen/RealSenseOpenCV.hpp DESTINATION include)
install(FILES OpenPose/OpenPoseCommandLine.hpp DESTINATION include)
install(FILES SystemInterface/SystemExec.hpp SystemInterface/CLInput.hpp SystemInterface/FileDeleter.hpp Utilities/SyncQueue.hpp DESTINATION include)
install(FILES Output/Skeleton.hpp DESTINATION include)
install(FILES Points/Point.hpp Points/Point3D.hpp Points/BodyKeyPoint.hpp DESTINATION include)
install(FILES Room/CoordinateMapping/CoordinateMappingManager.hpp Room/Room.hpp Room/CVPR_Lab.hpp DESTINATION include) 
install(FILES Blockchain/BlockchainLoader.hpp DESTINATION include)
install(FILES Kafka/KafkaManager.hpp Kafka/KafkaJSONSend.hpp DESTINATION include)