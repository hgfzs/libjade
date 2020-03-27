TEMPLATE = lib
TARGET = jade

DESTDIR = lib
INCLUDEPATH += include

CONFIG += release warn_on embed_manifest_dll c++11 qt staticlib
CONFIG -= debug

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

!win32:MOC_DIR = release
!win32:OBJECTS_DIR = release
!win32:RCC_DIR = release

# --------------------------------------------------------------------------------------------------

SOURCES += \
	source/DrawingArcItem.cpp \
	source/DrawingCurveItem.cpp \
	source/DrawingEllipseItem.cpp \
	source/DrawingItem.cpp \
	source/DrawingItemGroup.cpp \
	source/DrawingItemPoint.cpp \
	source/DrawingLineItem.cpp \
	source/DrawingPathItem.cpp \
	source/DrawingPolygonItem.cpp \
	source/DrawingPolylineItem.cpp \
	source/DrawingRectItem.cpp \
	source/DrawingTextItem.cpp \
	source/DrawingTextEllipseItem.cpp \
	source/DrawingTextPolygonItem.cpp \
	source/DrawingTextRectItem.cpp \
	source/DrawingScene.cpp \
	source/DrawingUndo.cpp \
	source/DrawingView.cpp

HEADERS += \
	include/DrawingArcItem.h \
	include/DrawingCurveItem.h \
	include/DrawingEllipseItem.h \
	include/DrawingItem.h \
	include/DrawingItemGroup.h \
	include/DrawingItemPoint.h \
	include/DrawingLineItem.h \
	include/DrawingPathItem.h \
	include/DrawingPolygonItem.h \
	include/DrawingPolylineItem.h \
	include/DrawingRectItem.h \
	include/DrawingTextItem.h \
	include/DrawingTextEllipseItem.h \
	include/DrawingTextPolygonItem.h \
	include/DrawingTextRectItem.h \
	include/DrawingScene.h \
	include/DrawingUndo.h \
	include/DrawingView.h \
    include/Drawing.h

# --------------------------------------------------------------------------------------------------

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
