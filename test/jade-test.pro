TEMPLATE = app

win32:TARGET = JadeTest
!win32:TARGET = jade_test

DESTDIR = bin
INCLUDEPATH += .

CONFIG += release warn_on embed_manifest_dll c++11 qt
CONFIG -= debug
QT += widgets

!win32:MOC_DIR = release
!win32:OBJECTS_DIR = release
!win32:RCC_DIR = release

INCLUDEPATH += ../include
win32:LIBS += ../build/lib/jade.lib
!win32:LIBS += -L../build/lib -ljade

# --------------------------------------------------------------------------------------------------

SOURCES += \
    DiagramWidget.cpp \
    DiagramWindow.cpp \
    main.cpp

HEADERS += \
    DiagramWidget.h \
    DiagramWindow.h
