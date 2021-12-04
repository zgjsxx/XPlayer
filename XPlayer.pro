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
        src/CXAudioPlay.cpp \
        src/Logger.cpp \
        src/MainApp.cpp \
        src/XAudioPlay.cpp \
        src/XAudioThread.cpp \
        src/XDecode.cpp \
        src/XDecodeThread.cpp \
        src/XDemux.cpp \
        src/XDemuxThread.cpp \
        src/XPlay.cpp \
        src/XResample.cpp \
        src/XSubTitle.cpp \
        src/XVideoOutput.cpp \
        src/XVideoThread.cpp \
        src/main.cpp

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
    include/CXAudioPlay.h \
    include/IVideoCall.h \
    include/Logger.h \
    include/LoggerTemplate.h \
    include/MainApp.h \
    include/XAudioPlay.h \
    include/XAudioThread.h \
    include/XDecode.h \
    include/XDecodeThread.h \
    include/XDemux.h \
    include/XDemuxThread.h \
    include/XPlay.h \
    include/XResample.h \
    include/XSubTitle.h \
    include/XVideoOutput.h \
    include/XVideoThread.h


#Include path
INCLUDEPATH += ./import/ffmpeg/include \
                ./include

# Libaray path and libaray


LIBS += -L$$PWD/import/ffmpeg/lib/debug/x64/ -lavformat -lavcodec -lavcodec -lavutil -lswresample -lswscale


