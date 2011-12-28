QT                 += core testlib
QT                 -= gui
TARGET              = Simulator
CONFIG             += console
CONFIG             -= app_bundle
TEMPLATE            = app


QMAKE_CXXFLAGS     += -std=gnu++0x


SOURCES +=                                                            \
  src/main.cpp

HEADERS +=                                                            \
  src/types.h                                                         \
  src/interfaces/IPhysicalConnection.h
