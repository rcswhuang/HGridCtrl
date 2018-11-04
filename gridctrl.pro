#-------------------------------------------------
#
# Project created by huangw 2018-10-23T10:06:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gridctrl
TEMPLATE = app

INCLUDEPATH += \
                ../include \

SOURCES +=  hgridcellbase.cpp \
            hgridcell.cpp   \
            hgridctrl.cpp \
            hinplaceedit.cpp  \
            main.cpp

HEADERS  +=    \
            hcellrange.h \
            hgridcelldef.h \
            hgridcellbase.h \
            hgridcell.h     \
            hinplaceedit.h  \
            hgridctrl.h


unix{
    UI_DIR = temp/ui
    MOC_DIR = temp/moc
    OBJECTS_DIR	= temp/obj
}

win32{
    UI_DIR = temp/ui
    MOC_DIR = temp/moc
    OBJECTS_DIR	= temp/obj
}
