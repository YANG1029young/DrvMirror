#-------------------------------------------------
#
# Project created by QtCreator 2019-01-22T00:54:05
#
#-------------------------------------------------

QT       -= gui

TARGET = DrvMirror
TEMPLATE = lib

DEFINES += DRVMIRROR_LIBRARY

SOURCES += drvmirror.cpp \
    calibration.cpp \
    filehandle.cpp \
    common.cpp \
    getinfor.cpp \
    output.cpp \
    outputserve.cpp \
    objdetection.cpp

INCLUDEPATH += /usr/local/include \
/usr/local/include/opencv \
/usr/local/include/opencv2 \

LIBS += /usr/local/lib/libopencv_calib3d.so \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_dnn.so \
/usr/local/lib/libopencv_features2d.so \
/usr/local/lib/libopencv_flann.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_imgcodecs.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_objdetect.so \
/usr/local/lib/libopencv_photo.so \
/usr/local/lib/libopencv_shape.so \
/usr/local/lib/libopencv_stitching.so \
/usr/local/lib/libopencv_superres.so \
/usr/local/lib/libopencv_video.so \
/usr/local/lib/libopencv_videoio.so \
/usr/local/lib/libopencv_videostab.so \
/usr/local/lib/libopencv_xfeatures2d.so \
/usr/local/lib/libopencv_xfeatures2d.so \


HEADERS += drvmirror.h\
        drvmirror_global.h \
    calibration.h \
    filehandle.h \
    common.h \
    getinfor.h \
    output.h \
    outputserve.h \
    objdetection.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
