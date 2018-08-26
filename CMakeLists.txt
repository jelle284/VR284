cmake_minimum_required (VERSION 2.8)

#project name: VR284
project(VR284)

#set include
FILE(GLOB_RECURSE HEADER_FILES_GLOB include/*.h src/*.h)
set(HEADER_FILES include)
set(SOURCE_FILES src/VR284.cpp src/hmd.cpp src/hand_controller.cpp src/server_provider.cpp src/driverlog.cpp src/tracking_client.cpp)
include_directories(${HEADER_FILES})

#set output path
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE VR284/bin/win${BITS})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE VR284/bin/win${BITS})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE VR284/bin/win${BITS})

#find lib absolute path
#find_library(GLOG_LIBRARY libglog.lib HINTS third_party/glog/lib)


#copy dll to output directory
#file(COPY  third_party/glog/lib/libglog.dll DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE})

#copy resources to driver directory
file(COPY resources/icons DESTINATION VR284/resources)
file(COPY resources/settings DESTINATION VR284/resources)
file(COPY resources/driver.vrdrivermanifest DESTINATION VR284)

#generate dll
add_library(driver_VR284 SHARED ${SOURCE_FILES} ${HEADER_FILES} ${HEADER_FILES_GLOB})

#set link libraries
#target_link_libraries(driver_VR284 ${GLOG_LIBRARY})