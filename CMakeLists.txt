cmake_minimum_required (VERSION 2.6)
set( CMAKE_CXX_FLAGS "-O3 -Wall ${CMAKE_CXX_FLAGS}" )

# SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "CMakeCache.txt;CMakeFiles;cmake_install.cmake")

# linux specific stuff

IF ( UNIX )
   set( CMAKE_PREFIX_PATH "/opt/opencv" )
   set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
   MESSAGE( "LINUX CONFIG" )
ENDIF ( UNIX )

# windows (inc. 64-bit specific stuff)

IF ( WIN32 )
   set( CMAKE_PREFIX_PATH "C:/OpenCV/build" )
   set( OpenCV_DIR "C:/OpenCV/build" )
   MESSAGE( "WINDOWS CONFIG" )
ENDIF ( WIN32 )

find_package( OpenCV REQUIRED )

project(colourquery)
add_executable(colourquery colourquery.cpp)
target_link_libraries( colourquery ${OpenCV_LIBS} )

project(displayimage)
add_executable(displayimage displayimage.cpp)
target_link_libraries( displayimage ${OpenCV_LIBS} )

project(liveimage)
add_executable(liveimage liveimage.cpp)
target_link_libraries( liveimage ${OpenCV_LIBS} )

project(livevideo)
add_executable(livevideo livevideo.cpp)
target_link_libraries( livevideo ${OpenCV_LIBS} )

project(saveimage)
add_executable(saveimage saveimage.cpp)
target_link_libraries( saveimage ${OpenCV_LIBS} )

project(smoothimage)
add_executable(smoothimage smoothimage.cpp)
target_link_libraries( smoothimage ${OpenCV_LIBS} )

project(writevideo)
add_executable(writevideo writevideo.cpp)
target_link_libraries( writevideo ${OpenCV_LIBS} )

find_package( OpenCV REQUIRED )

project(bg_fg_mog)
add_executable(bg_fg_mog bg_fg_mog.cpp)
target_link_libraries( bg_fg_mog ${OpenCV_LIBS} )

project(butterworth_lowpass)
add_executable(butterworth_lowpass butterworth_lowpass.cpp)
target_link_libraries( butterworth_lowpass ${OpenCV_LIBS} )

project(fourier)
add_executable(fourier fourier.cpp)
target_link_libraries( fourier ${OpenCV_LIBS} )

project(generic_interface)
add_executable(generic_interface generic_interface.cpp)
target_link_libraries( generic_interface ${OpenCV_LIBS} )

project(generic_recognition_interface)
add_executable(generic_recognition_interface generic_recognition_interface.cpp)
target_link_libraries( generic_recognition_interface ${OpenCV_LIBS} )

project(generic_selection_interface)
add_executable(generic_selection_interface generic_selection_interface.cpp)
target_link_libraries( generic_selection_interface ${OpenCV_LIBS} )

project(harris)
add_executable(harris harris.cpp)
target_link_libraries( harris ${OpenCV_LIBS} )

project(histogram_based_recognition_colour)
add_executable(histogram_based_recognition_colour histogram_based_recognition_colour.cpp)
target_link_libraries( histogram_based_recognition_colour ${OpenCV_LIBS} )

project(histogram_based_recognition)
add_executable(histogram_based_recognition histogram_based_recognition.cpp)
target_link_libraries( histogram_based_recognition ${OpenCV_LIBS} )

project(meanshift_segmentation)
add_executable(meanshift_segmentation meanshift_segmentation.cpp)
target_link_libraries( meanshift_segmentation ${OpenCV_LIBS} )

project(opencv_c_from_cpp)
add_executable(opencv_c_from_cpp opencv_c_from_cpp.cpp)
set_target_properties(opencv_c_from_cpp PROPERTIES COMPILE_FLAGS "-fpermissive")
target_link_libraries( opencv_c_from_cpp ${OpenCV_LIBS} )

project(polygons)
add_executable(polygons polygons.cpp)
target_link_libraries( polygons ${OpenCV_LIBS} )

project(nlm)
add_executable(nlm nlm.cpp)
target_link_libraries( nlm ${OpenCV_LIBS} )

project(nlm2)
add_executable(nlm2 nlm2.cpp)
target_link_libraries( nlm2 ${OpenCV_LIBS} )

project(mean_filter)
add_executable(mean_filter mean_filter.cpp)
target_link_libraries( mean_filter ${OpenCV_LIBS} )

project(bilateral_filter)
add_executable(bilateral_filter bilateral_filter.cpp)
target_link_libraries( bilateral_filter ${OpenCV_LIBS} )

project(optical_flow_fback)
add_executable(optical_flow_fback optical_flow_fback.cpp)
target_link_libraries( optical_flow_fback ${OpenCV_LIBS} )
