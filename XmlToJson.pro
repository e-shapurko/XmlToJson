TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++1z

SOURCES += \
        main.cpp \
    TinyXML/tinystr.cpp \
    TinyXML/tinyxml.cpp \
    TinyXML/tinyxmlerror.cpp \
    TinyXML/tinyxmlparser.cpp \
    Convertor/convertor.cpp

HEADERS += \
    TinyXML/tinystr.h \
    TinyXML/tinyxml.h \
    Convertor/convertor.h
