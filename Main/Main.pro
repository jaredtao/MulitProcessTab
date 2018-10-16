TEMPLATE = app
TARGET= Main
include($$PWD/../common.pri)
LIBS += -luser32
SOURCES += \
    Src/main.cpp \
    Src/TabMgr.cpp \
    Src/ProcessMgr.cpp \
    Src/IPC.cpp

RESOURCES += \
    Qml.qrc

HEADERS += \
    Src/TabMgr.h \
    Src/ProcessMgr.h \
    Src/IPC.h \
    Src/Common.h
