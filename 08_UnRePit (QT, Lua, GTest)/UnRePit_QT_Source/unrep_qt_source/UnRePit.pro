QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    unrep_main.cpp \
    unrep_reformer.cpp \
    unrep_ui_stringspinbox.cpp \
    unrep_winmain.cpp

HEADERS += \
    unrep_luafunc.h \
    unrep_reformer.h \
    unrep_tokensearch.h \
    unrep_ui_stringspinbox.h \
    unrep_winmain.h

FORMS += \
    unrep_winmain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT

win32: LIBS += -L$$PWD/LuaLibs/ -llua54_x64

INCLUDEPATH += $$PWD/LuaLibs
DEPENDPATH += $$PWD/LuaLibs

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/LuaLibs/lua54_x64.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/LuaLibs/liblua54_x64.a

win32: LIBS += -L$$PWD/ -lunrep_tokensearch

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/unrep_tokensearch.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/libunrep_tokensearch.a

DISTFILES +=

RESOURCES += \
    unrep_resourses.qrc

RC_ICONS = unrep_icon.ico
