#-------------------------------------------------
#
# Project created by QtCreator 2017-05-17T11:06:23
#
#-------------------------------------------------

QT       += core gui
QT       += opengl
QT       += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OffsetViewer
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
        myvideosurface.cpp \
        videowidget.cpp \
        camera.cpp \
        shared.cpp \
        scanthread.cpp \
        config.cpp

HEADERS  += widget.h \
            myvideosurface.h \
            videowidget.h \
            camera.h \
            shared.h \
            scanthread.h \
            config.h

FORMS    += widget.ui \
            config.ui

RESOURCES += \
    resource.qrc

macx{
    #libdmtx
    INCLUDEPATH += /usr/local/include
    #LIBS += `pkg-config --libs libdmtx`
    LIBS += -L/usr/local/lib -ldmtx
}
unix:!macx{
    #preprocessor definition
    DEFINES += RASPBERRY

    #libdmtx
    INCLUDEPATH += /home/levy/raspi/sysroot/usr/include
    LIBS += -L/home/levy/raspi/sysroot/usrlib -ldmtx

    #Cross Compile
    target.path = /home/pi/test
    INSTALLS += target
}
win32{
    #preprocessor definition
    DEFINES += WINDOWS

    #libdmtx
    INCLUDEPATH += C:\libdmtx-libdmtx\include
    LIBS += C:\libdmtx-libdmtx\libdmtx.lib
}
