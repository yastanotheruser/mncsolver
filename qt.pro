TEMPLATE = app
TARGET = app.out

QT = core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt debug
HEADERS += MclWindow.hpp LabelRow.hpp MclWidget.hpp mcl.hpp
SOURCES += main.cpp MclWindow.cpp LabelRow.cpp MclWidget.cpp mcl.cpp

latexsvg.commands = @make -C latex

QMAKE_EXTRA_TARGETS += latexsvg
PRE_TARGETDEPS += latexsvg
