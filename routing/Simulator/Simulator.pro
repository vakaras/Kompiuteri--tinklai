QT += core testlib
QT -= gui
TARGET = Simulator
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
DEFINES += QT_FATAL_ASSERT


QMAKE_CXXFLAGS += -std=gnu++0x

SOURCES +=                                                  \
  main.cpp                                                  \
  cable/connectionpoint.cpp                                 \
  cable/testcable.cpp                                       \
  cable/cable.cpp                                           \
  cable/cableprocess.cpp                                    \
  utils/executor.cpp                                        \
  macsublayer/macsublayer.cpp                               \
  macsublayer/macsublayerframe.cpp                          \
  macsublayer/macreader.cpp                                 \
  macsublayer/testmacsublayer.cpp                           \
  utils/random.cpp

HEADERS +=                                                  \
  types.h                                                   \
  interfaces/IPhysicalConnection.h                          \
  cable/connectionpoint.h                                   \
  cable/testcable.h                                         \
  cable/cable.h                                             \
  cable/cableprocess.h                                      \
  utils/executor.h                                          \
  interfaces/IMACSublayer.h                                 \
  macsublayer/macsublayer.h                                 \
  macsublayer/macsublayerframe.h                            \
  macsublayer/macreader.h                                   \
  macsublayer/testmacsublayer.h                             \
  utils/random.h
