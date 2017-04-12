project(MOTDemo)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
find_package( OpenCV REQUIRED )

SET (CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11")

#INCLUDE_DIRECTORIES("./bgslibrary/package_bgs/")
include_directories("./bgslibrary/package_bgs/")

set(OpenCV_LIBS
    libopencv_core.so
#    libopencv_face.so
    libopencv_highgui.so
    libopencv_imgcodecs.so
    libopencv_imgproc.so
#    libopencv_calib3d.so
    libopencv_features2d.so
#    libopencv_flann.so
    libopencv_imgproc.so
#    libopencv_ml.so
#    libopencv_objdetect.so
#    libopencv_photo.so
#    libopencv_shape.so
#    libopencv_stitching.so
#    libopencv_superres.so
    libopencv_video.so
    libopencv_videoio.so
#    libopencv_videostab.so
    libopencv_tracking.so
    )
target_link_libraries( MOTDemo ${OpenCV_LIBS} -ltiff /home/youyoujushi/文档/workspace/opencv/MOTDemo/bgslibrary/libbgs.a)
