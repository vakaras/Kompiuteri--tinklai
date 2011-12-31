QT += core testlib
QT -= gui
TARGET = Simulator
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app


QMAKE_CXXFLAGS += -std=gnu++0x

SOURCES +=                                                  \
  main.cpp                                                  \
  cable/cable.cpp                                           \
    cable/connectionpoint.cpp

HEADERS +=                                                  \
  types.h                                                   \
  interfaces/IPhysicalConnection.h                          \
  cable/cable.h                                             \
    cable/connectionpoint.h
