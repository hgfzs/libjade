TEMPLATE = lib

win32:TARGET = jade
!win32:TARGET = jade

DESTDIR = bin
INCLUDEPATH += include

CONFIG += release warn_on embed_manifest_dll c++11 qt
CONFIG -= debug
QT += widgets printsupport svg

!win32:MOC_DIR = release
!win32:OBJECTS_DIR = release
!win32:RCC_DIR = release

# --------------------------------------------------------------------------------------------------

SOURCES += \
	source/DrawingItem.cpp \
	source/DrawingItemGroup.cpp \
	source/DrawingItemPoint.cpp \
    source/DrawingItemStyle.cpp \
    source/DrawingPathItem.cpp \
	source/DrawingShapeItems.cpp \
	source/DrawingTextItem.cpp \
	source/DrawingTwoPointItems.cpp \
	source/DrawingTypes.cpp \
	source/DrawingUndo.cpp \
    source/DrawingWidget.cpp

HEADERS += \
    include/DrawingItem.h \
    include/DrawingItemGroup.h \
    include/DrawingItemPoint.h \
    include/DrawingItemStyle.h \
    include/DrawingPathItem.h \
    include/DrawingShapeItems.h \
    include/DrawingTextItem.h \
    include/DrawingTwoPointItems.h \
    include/DrawingTypes.h \
	include/DrawingUndo.h \
	include/DrawingWidget.h \
	include/Drawing
