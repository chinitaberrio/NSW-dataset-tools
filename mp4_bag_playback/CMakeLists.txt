cmake_minimum_required(VERSION 2.8.3)
project(mp4_bag_playback)

find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED COMPONENTS common)
find_package(tf2_eigen REQUIRED)

find_package(catkin REQUIRED COMPONENTS nodelet
	rosbag
	#rosbag_storage
	gmsl_frame_msg
	roscpp
	tf2_ros
	tf2_sensor_msgs
	image_transport
	dataset_msgs
	pcl_conversions
	std_msgs
	cv_bridge
	nav_msgs
	ouster_ros
	sensor_msgs)

find_package(OpenCV REQUIRED)

set(_ouster_ros_INCLUDE_DIRS
  "../../ouster_example/ouster_client/include;../../ouster_example/ouster_client/include/optional-lite")

## Setup include directories
include_directories(${catkin_INCLUDE_DIRS}, 
  ${_ouster_ros_INCLUDE_DIRS}
  include/${PROJECT_NAME}
  include)



catkin_package(
  LIBRARIES ${PROJECT_NAME}
	INCLUDE_DIRS include ${_ouster_ros_INCLUDE_DIRS}
	CATKIN_DEPENDS nodelet
	roscpp
	rosbag
	rosbag_storage
	gmsl_frame_msg
	image_transport
	dataset_msgs
	std_msgs
	tf2_ros
	tf2_sensor_msgs
	nav_msgs
	ouster_ros
	sensor_msgs
	cv_bridge
)

set(_SAVE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(../../ouster_example/ouster_client ouster_client EXCLUDE_FROM_ALL)
target_compile_options(ouster_client PRIVATE -Wno-deprecated-declarations)
message(STATUS "Ouster SDK client: Using EIGEN_MAX_ALIGN_BYTES = 32")
target_compile_definitions(ouster_client PUBLIC EIGEN_MAX_ALIGN_BYTES=32)
set(BUILD_SHARED_LIBS ${_SAVE_BUILD_SHARED_LIBS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# Create the nodelet tutorial library
add_executable(${PROJECT_NAME}_node
		src/mp4_bag_playback.cpp
		src/video.cpp
		src/helper_functions.cpp)

target_link_libraries(${PROJECT_NAME}_node
		${catkin_LIBRARIES}
		${OpenCV_LIBS})

add_library(${PROJECT_NAME}
		src/mp4_bag_playback.cpp
		src/video.cpp
		src/helper_functions.cpp
		src/bag_container.cpp)

target_link_libraries(${PROJECT_NAME}
		${catkin_LIBRARIES}
		${OpenCV_LIBS})


if(catkin_EXPORTED_LIBRARIES)
  add_dependencies(${PROJECT_NAME}
		  ${catkin_EXPORTED_LIBRARIES}
		  ${catkin_LIBRARIES}
		  ${OpenCV_LIBS})
endif()


## Mark other files for installation (e.g. launch and bag files, etc.)
install(FILES nodelet_definition.xml 
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

## Mark executables and/or libraries for installation
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch
		DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

install(DIRECTORY config
		DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

install(DIRECTORY include/${PROJECT_NAME}/
		DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})



## Add gtest based cpp test target and link libraries
get_filename_component(BAG_TEST_FOLDER "test/" ABSOLUTE)
configure_file(test/bag_data.h.in test/bag_data.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/test)

catkin_add_gtest(${PROJECT_NAME}-test
		test/test_playback.cpp
		)

if(TARGET ${PROJECT_NAME}-test)
	target_link_libraries(${PROJECT_NAME}-test mp4_bag_playback)
endif()
