QT += qml quick gui widgets multimedia opengl openglextensions

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    RC_FILE = resource/win32/XPlayer.rc
}

SOURCES += \
        CXAudioPlay.cpp \
        Logger.cpp \
        MainApp.cpp \
        XAudioPlay.cpp \
        XAudioThread.cpp \
        XDecode.cpp \
        XDecodeThread.cpp \
        XDemux.cpp \
        XDemuxThread.cpp \
        XPlay.cpp \
        XResample.cpp \
        XSubTitle.cpp \
        XVideoOutput.cpp \
        XVideoThread.cpp \
        main.cpp

RESOURCES += \
    resource/XPlayer.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CXAudioPlay.h \
    IVideoCall.h \
    Logger.h \
    LoggerTemplate.h \
    MainApp.h \
    XAudioPlay.h \
    XAudioThread.h \
    XDecode.h \
    XDecodeThread.h \
    XDemux.h \
    XDemuxThread.h \
    XPlay.h \
    XResample.h \
    XSubTitle.h \
    XVideoOutput.h \
    XVideoThread.h


#Include path
INCLUDEPATH += ./import/ffmpeg/include

# Libaray path and libaray


LIBS += -L$$PWD/import/ffmpeg/lib/debug/x64/ -lavformat -lavcodec -lavcodec -lavutil -lswresample -lswscale


