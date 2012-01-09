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
  llcsublayer/llcsublayer.cpp                               \
  networklayer/networklayer.cpp                             \
  networklayer/neighbourinfo.cpp                            \
  networklayer/testnetworklayer.cpp                         \
  networklayer/routingprocess.cpp                           \
  networklayer/helloneighbourpacket.cpp                     \
  networklayer/networkreader.cpp                            \
  networklayer/connectionwrapper.cpp                        \
  networklayer/neighbourinfopacket.cpp                      \

HEADERS +=                                                  \
  types.h                                                   \
  utils/random.h                                            \
  utils/executor.h                                          \
  utils/sharedarraydeleter.h                                \
  interfaces/IPhysicalConnection.h                          \
  interfaces/IMACSublayer.h                                 \
  interfaces/ILLCSublayer.h                                 \
  interfaces/INetworkLayer.h                                \
  cable/connectionpoint.h                                   \
  cable/testcable.h                                         \
  cable/cable.h                                             \
  cable/cableprocess.h                                      \
  macsublayer/macsublayer.h                                 \
  macsublayer/macsublayerframe.h                            \
  macsublayer/macreader.h                                   \
  macsublayer/testmacsublayer.h                             \
  llcsublayer/llcsublayer.h                                 \
  networklayer/networklayer.h                               \
  networklayer/neighbourinfo.h                              \
  networklayer/testnetworklayer.h                           \
  networklayer/routingprocess.h                             \
  networklayer/helloneighbourpacket.h                       \
  networklayer/networkreader.h                              \
  networklayer/connectionwrapper.h                          \
  networklayer/neighbourinfopacket.h                        \
