QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    INCLUDEPATH += ./include
    LIBS += ./lib/obs.lib
    Debug:OBJECTS_DIR = ./build/obj_debug
    Release:OBJECTS_DIR = ./build/obj_release
}

# linux {
#     INCLUDEPATH += /home/rvd/Documents/dev/obs-studio/libobs
#     LIBS += /home/rvd/Documents/dev/obs-studio/build_ubuntu/libobs/libobs.so
#     DESTDIR = ./dest
#     OBJECTS_DIR = ./build/obj
# }

UI_DIR = ./build/ui
MOC_DIR = ./build/moc

SOURCES += \
    src/capturedevicepicker.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/obswidget.cpp

HEADERS += \
    src/capturedevicepicker.h \
    src/mainwindow.h \
    src/obswidget.h

FORMS += \
    src/capturedevicepicker.ui \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
