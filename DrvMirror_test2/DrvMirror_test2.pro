QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp

INCLUDEPATH += ./DrvMirror/include \

LIBS += ./DrvMirror/lib/opencv2/libopencv_calib3d.so \
./DrvMirror/lib/opencv2/libopencv_core.so \
./DrvMirror/lib/opencv2/libopencv_dnn.so \
./DrvMirror/lib/opencv2/libopencv_flann.so \
./DrvMirror/lib/opencv2/libopencv_highgui.so \
./DrvMirror/lib/opencv2/libopencv_imgcodecs.so \
./DrvMirror/lib/opencv2/libopencv_imgproc.so \
./DrvMirror/lib/opencv2/libopencv_ml.so \
./DrvMirror/lib/opencv2/libopencv_objdetect.so \
./DrvMirror/lib/opencv2/libopencv_photo.so \
./DrvMirror/lib/opencv2/libopencv_shape.so \
./DrvMirror/lib/opencv2/libopencv_stitching.so \
./DrvMirror/lib/opencv2/libopencv_superres.so \
./DrvMirror/lib/opencv2/libopencv_video.so \
./DrvMirror/lib/opencv2/libopencv_videoio.so \
./DrvMirror/lib/opencv2/libopencv_videostab.so \
./DrvMirror/lib/opencv2/libopencv_xfeatures2d.so \
./DrvMirror/lib/opencv2/libopencv_xfeatures2d.so \


HEADERS += \
    ../build-DrvMirror_test2-Desktop-Debug/DrvMirror/include/drvmirror_global.h \
    ../build-DrvMirror_test2-Desktop-Debug/DrvMirror/include/drvmirror.h \
    ../build-DrvMirror_test2-Desktop-Debug/DrvMirror/include/common.h







