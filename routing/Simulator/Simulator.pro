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
  utils/executor.cpp                                        \
  utils/random.cpp                                          \
  cable/connectionpoint.cpp                                 \
  cable/testcable.cpp                                       \
  cable/cable.cpp                                           \
  cable/cableprocess.cpp                                    \
  macsublayer/macsublayer.cpp                               \
  macsublayer/macsublayerframe.cpp                          \
  macsublayer/macreader.cpp                                 \
  macsublayer/testmacsublayer.cpp                           \

HEADERS +=                                                  \
  types.h                                                   \
  utils/random.h                                            \
  utils/executor.h                                          \
  utils/sharedarraydeleter.h                                \
  interfaces/IPhysicalConnection.h                          \
  cable/connectionpoint.h                                   \
  cable/testcable.h                                         \
  cable/cable.h                                             \
  cable/cableprocess.h                                      \
  interfaces/IMACSublayer.h                                 \
  macsublayer/macsublayer.h                                 \
  macsublayer/macsublayerframe.h                            \
  macsublayer/macreader.h                                   \
  macsublayer/testmacsublayer.h                             \

