TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    uwcpu.cpp \
    ram.cpp \
    uwassembler.cpp \
    uwsim.cpp

HEADERS += \
    uwcpu.h \
    ram.h \
    uwassembler.h \
    uwsim.h

DISTFILES += \
    packuw.uwasm \
    simplest.uwasm \
    subsetsum.uwasm \
    data_subsum.txt

INCLUDEPATH += C:\Users\cmpgt\OneDrive\Documentos\boost_1_64_0\boost_1_64_0
