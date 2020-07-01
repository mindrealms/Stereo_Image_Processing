#-------------------------------------------------
#
# Project created by QtCreator 2020-06-23T00:37:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = depth_map_reproj
TEMPLATE = app

INCLUDEPATH +=  /Users/marky/OpenCV/OpenCV_source/include/opencv2 \
                /usr/local/include/opencv4 \
                /Users/marky/Open3D/cpp \
                /Users/marky/Eigen/eigen-3.3.7 \
                /Users/marky/Open3D/3rdparty/glew/include \
                /Users/marky/Open3D/3rdparty/GLFW/include \
                /Users/marky/Open3D/3rdparty/fmt/include

LIBS += -L"/usr/local/lib" \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d\
     -lopencv_highgui \
     -lopencv_features2d \
     -lopencv_photo \
     -lopencv_imgcodecs \
     -lopencv_calib3d \
     -L"/Users/marky/Open3D/build/lib"
#     -lopencv_contrib \
#     -lopencv_legacy \
#     -l \
#     -lopencv_world

#LIBS += `pkg-config --libs opencv`

QT_CONFIG -= no-pkg-config
CONFIG+=link_pkgconfig \
           c++11
#PKGCONFIG+=opencv #having a hard time linking cv and eigen through pkg-config & .pc files for some reason :(

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    PfmLoader.cpp \
    StereoUtils.cpp \
    CalibLoader.cpp

HEADERS  += \
    PfmLoader.h \
    StereoUtils.h \
    CalibLoader.h
