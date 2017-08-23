QT += core widgets

TARGET = 277
TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG += warn_on
CONFIG += debug
QT += multimedia


INCLUDEPATH += include

include(src/src.pri)

FORMS += forms/mainwindow.ui \
    forms/cameracontrolshelp.ui

RESOURCES += glsl.qrc

*-clang*|*-g++* {
    message("Enabling additional warnings")
    CONFIG -= warn_on
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -Winit-self
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -fno-omit-frame-pointer
}
linux-clang*|linux-g++*|macx-clang*|macx-g++* {
    message("Enabling stack protector")
    QMAKE_CXXFLAGS += -fstack-protector-all
}

# FOR LINUX & MAC USERS INTERESTED IN ADDITIONAL BUILD TOOLS
# ----------------------------------------------------------
# This conditional exists to enable Address Sanitizer (ASAN) during
# the automated build. ASAN is a compiled-in tool which checks for
# memory errors (like Valgrind). You may enable it for yourself;
# check the hidden `.build.sh` file for info. But be aware: ASAN may
# trigger a lot of false-positive leak warnings for the Qt libraries.
# (See `.run.sh` for how to disable leak checking.)
address_sanitizer {
    message("Enabling Address Sanitizer")
    QMAKE_CXXFLAGS += -fsanitize=address
    QMAKE_LFLAGS += -fsanitize=address
}

HEADERS +=

SOURCES +=

QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
LIBS += -framework CoreFoundation
LIBS += -framework OpenGL




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libSOIL-master/release/ -lSOIL
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libSOIL-master/debug/ -lSOIL
else:unix: LIBS += -L$$PWD/../libSOIL-master/ -lSOIL

INCLUDEPATH += $$PWD/../libSOIL-master
DEPENDPATH += $$PWD/../libSOIL-master

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libSOIL-master/release/libSOIL.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libSOIL-master/debug/libSOIL.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libSOIL-master/release/SOIL.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libSOIL-master/debug/SOIL.lib
else:unix: PRE_TARGETDEPS += $$PWD/../libSOIL-master/libSOIL.a

DISTFILES += \
    ../person_running_on_snow.mp3
