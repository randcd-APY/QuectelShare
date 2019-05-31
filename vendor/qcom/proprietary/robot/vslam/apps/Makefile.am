ACLOCAL_AMFLAGS = -I m4

# mv_vwslam
# =========================================================
bin_PROGRAMS = mv_vwslam
mv_vwslam_SOURCES := \
     src/main.cpp \
     src/mainThread.cpp \
     src/Camera_VSLAM.cpp \
     src/ReadImages.cpp \
     src/ReadConfigurationFile.cpp \
     src/ReadConfigurationFile_Internal.cpp \
     src/ReadGrayBMP.cpp \
     src/ScaleEstimation.cpp \
     src/Visualization.cpp \
     src/Visualization_NonRos.cpp \
     src/UndistortionEngine.cpp \
     src/VSLAM.cpp \
     src/WEF.cpp  \
     src/VSLAMScheduler.cpp \
     src/SystemTime.cpp \
     src/EagleImu.cpp \
     src/EagleWheel.cpp \
     src/imuClient.cpp \
     src/Pose6DRT.cpp \
     src/PathPlanning_ROS.cpp \
     src/NaviVSLAM.cpp

mv_vwslam_CPPFLAGS = -O2
mv_vwslam_CPPFLAGS += -g
mv_vwslam_CPPFLAGS += -Wno-all
mv_vwslam_CPPFLAGS += -Wno-error
mv_vwslam_CPPFLAGS += -Wno-unused-parameter
mv_vwslam_CPPFLAGS += -I .
mv_vwslam_CPPFLAGS += -Iinc
mv_vwslam_CPPFLAGS += -I../prebuilt/inc
mv_vwslam_CPPFLAGS += -std=c++11
mv_vwslam_CPPFLAGS += -DARM_BASED
mv_vwslam_CPPFLAGS += -DIMU_SUPPORTED

mv_vwslam_LDADD = $(GLIB_LIBS)
mv_vwslam_LDADD += -ldl -lcutils -lpthread -lcamera
mv_vwslam_LDADD += ../prebuilt/libs/libmv0.so
mv_vwslam_LDADD += ../prebuilt/libs/libmv3.so
mv_vwslam_LDADD += ../prebuilt/libs/libmv1.so
#mv_vwslam_LDADD += -lopencv_core -lopencv_imgproc -lopencv_highgui

if USE_ROS
# mv_vwslam_ros
# =========================================================
bin_PROGRAMS += mv_vwslam_ros
mv_vwslam_ros_SOURCES := \
     src/Camera_VSLAM.cpp  \
     src/mainThread.cpp    \
     src/ReadImages.cpp  \
     src/ReadConfigurationFile.cpp  \
     src/ReadConfigurationFile_Internal.cpp  \
     src/ReadGrayBMP.cpp \
     src/ScaleEstimation.cpp  \
     src/Visualization.cpp  \
     src/UndistortionEngine.cpp  \
     src/VSLAM.cpp  \
     src/WEF.cpp  \
     src/ros_wrapper.cpp \
     src/Visualization_ROS.cpp \
     src/VirtualWheel_ROS.cpp  \
     src/VSLAMScheduler.cpp \
     src/SystemTime.cpp \
     src/EagleImu.cpp \
     src/EagleWheel.cpp \
     src/Pose6DRT.cpp \
     src/imuClient.cpp \
     src/PathPlanning_ROS.cpp \
     src/NaviVSLAM.cpp

mv_vwslam_ros_CPPFLAGS = -O2
mv_vwslam_ros_CPPFLAGS += -g
mv_vwslam_ros_CPPFLAGS += -Wno-all
mv_vwslam_ros_CPPFLAGS += -Wno-error
mv_vwslam_ros_CPPFLAGS += -Wno-unused-parameter
mv_vwslam_ros_CPPFLAGS += -I .
mv_vwslam_ros_CPPFLAGS += -Iinc
mv_vwslam_ros_CPPFLAGS += -I../prebuilt/inc
mv_vwslam_ros_CPPFLAGS += -std=c++11
mv_vwslam_ros_CPPFLAGS += -DARM_BASED
mv_vwslam_ros_CPPFLAGS += -DROS_BASED
mv_vwslam_ros_CPPFLAGS += -DIMU_SUPPORTED
mv_vwslam_ros_CPPFLAGS += -DWHEEL_SUPPORTED
mv_vwslam_ros_CPPFLAGS += -DWALL_DETECT
mv_vwslam_ros_CPPFLAGS += -DFEATURE_DETECT

mv_vwslam_ros_LDADD = $(GLIB_LIBS)
mv_vwslam_ros_LDADD += ../prebuilt/libs/libmv0.so
mv_vwslam_ros_LDADD += ../prebuilt/libs/libmv3.so
mv_vwslam_ros_LDADD += ../prebuilt/libs/libmv1.so
mv_vwslam_ros_LDADD += -ldl -lcutils -lpthread -lcamera
mv_vwslam_ros_LDADD += -lopencv_core -lopencv_imgproc -lopencv_highgui
mv_vwslam_ros_LDADD += -limage_transport -lrosconsole -lrosconsole_bridge
mv_vwslam_ros_LDADD += -lroscpp -lroslib -lroscpp_serialization -lrostime -lcpp_common
mv_vwslam_ros_LDADD += -lcv_bridge -lxmlrpcpp -lclass_loader -lmessage_filters
mv_vwslam_ros_LDADD += -lrospack -lrosconsole_log4cxx -lrosconsole_backend_interface

# wheel_navi
# =========================================================
bin_PROGRAMS += wheel_navi
wheel_navi_SOURCES := \
     src/NaviWheel.cpp  \
     src/PathPlanning_ROS.cpp \
     src/imuClient.cpp

wheel_navi_CPPFLAGS = -O2
wheel_navi_CPPFLAGS += -g
wheel_navi_CPPFLAGS += -Wno-all
wheel_navi_CPPFLAGS += -Wno-error
wheel_navi_CPPFLAGS += -Wno-unused-parameter
wheel_navi_CPPFLAGS += -I .
wheel_navi_CPPFLAGS += -Iinc
wheel_navi_CPPFLAGS += -I../prebuilt/inc
wheel_navi_CPPFLAGS += -std=c++11
wheel_navi_CPPFLAGS += -DROS_BASED
wheel_navi_CPPFLAGS += -DARM_BASED
wheel_navi_CPPFLAGS += -DIMU_SUPPORTED
wheel_navi_CPPFLAGS += -DOPENCV_SUPPORTED

wheel_navi_LDADD = $(GLIB_LIBS)
wheel_navi_LDADD += -ldl -lcutils -lpthread
wheel_navi_LDADD += -lopencv_core -lopencv_imgproc -lopencv_highgui
wheel_navi_LDADD += -limage_transport -lrosconsole -lrosconsole_bridge
wheel_navi_LDADD += -lroscpp -lroslib -lroscpp_serialization -lrostime -lcpp_common
wheel_navi_LDADD += -lcv_bridge -lxmlrpcpp -lclass_loader -lmessage_filters
wheel_navi_LDADD += -lrospack -lrosconsole_log4cxx -lrosconsole_backend_interface
endif
