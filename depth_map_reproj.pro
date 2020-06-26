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
                /usr/local/Cellar/boost/1.72.0_3 \
                /usr/local/include/opencv4 \ #pff why
                /usr/local/include

LIBS += -L"/usr/local/lib" \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d\
     -lopencv_highgui \
     -lopencv_features2d \
     -lopencv_photo \
     -lopencv_imgcodecs \
     -lopencv_calib3d
#     -lopencv_contrib \
#     -lopencv_legacy
#     -l
#        -lopencv_world

#LIBS += `pkg-config --libs opencv`

QT_CONFIG -= no-pkg-config
CONFIG+=link_pkgconfig \
           c++11
#PKGCONFIG+=opencv #I CAN'T LINK THIS IDK WHAT'S WRONG WITH MY PKG-CONFIG / OPENCV.PC FILE :(


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
        mainwindow.cpp \
        DepthMap.cpp

HEADERS  += mainwindow.h \
        DepthMap.h

FORMS    += mainwindow.ui
