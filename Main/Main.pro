TEMPLATE = app
TARGET= Main
include($$PWD/../common.pri)
SOURCES += \
    Src/main.cpp \
    Src/TabMgr.cpp

RESOURCES += \
    Qml.qrc

HEADERS += \
    Src/TabMgr.h
