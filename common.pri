CONFIG(debug, debug|release) {
    DESTDIR = $$absolute_path($$PWD/bin/Debug)
} else {
    DESTDIR = $$absolute_path($$PWD/bin/Release)
}
QT += core gui qml quick widgets

