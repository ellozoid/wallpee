#-------------------------------------------------
#
# Project created by QtCreator 2015-01-04T00:40:15
#
#-------------------------------------------------

QT       += core gui \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = wall
TEMPLATE = app


SOURCES += main.cpp\
        wall.cpp

HEADERS  += wall.h



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/QGumboParser/release/ -lQGumboParser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/QGumboParser/debug/ -lQGumboParser

INCLUDEPATH += $$PWD/../lib/QGumboParser
DEPENDPATH += $$PWD/../lib/QGumboParser

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QGumboParser/release/libQGumboParser.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QGumboParser/debug/libQGumboParser.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QGumboParser/release/QGumboParser.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QGumboParser/debug/QGumboParser.lib

RESOURCES += \
    tray.qrc
win32 {
    RC_FILE +=
    OTHER_FILES +=
}
