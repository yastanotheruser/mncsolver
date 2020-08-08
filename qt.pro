TEMPLATE = app
TARGET = app

QT = core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt debug release
HEADERS += MclWindow.hpp LabelRow.hpp MclWidget.hpp mcl.hpp parser.hpp
SOURCES += main.cpp MclWindow.cpp LabelRow.cpp MclWidget.cpp mcl.cpp parser.cpp

latexsvg.commands = @make -C latex

QMAKE_EXTRA_TARGETS += latexsvg
PRE_TARGETDEPS += latexsvg
