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
  utils/genericthread.cpp                                   \
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
  networklayer/routingtable.cpp                             \
  networklayer/routerinfo.cpp                               \
  networklayer/neighbourinfoackpacket.cpp                   \
  networklayer/routingtableprocess.cpp                      \
  networklayer/ippacket.cpp                                 \
  router/router.cpp                                         \
  transportlayer/transportlayer.cpp                         \
  transportlayer/socket.cpp                                 \
  transportlayer/tcppacket.cpp                              \
  transportlayer/testtransportlayer.cpp                     \
  transportlayer/transportreader.cpp                        \
  transportlayer/listener.cpp                               \
  host/host.cpp                                             \
  host/testhost.cpp                                         \

HEADERS +=                                                  \
  types.h                                                   \
  utils/random.h                                            \
  utils/executor.h                                          \
  utils/sharedarraydeleter.h                                \
  utils/genericthread.h                                     \
  interfaces/IPhysicalConnection.h                          \
  interfaces/IMACSublayer.h                                 \
  interfaces/ILLCSublayer.h                                 \
  interfaces/INetworkLayer.h                                \
  interfaces/ITransportLayer.h                              \
  cable/connectionpoint.h                                   \
  cable/testcable.h                                         \
  cable/cable.h                                             \
  cable/cableprocess.h                                      \
  macsublayer/macsublayer.h                                 \
  macsublayer/macsublayerframe.h                            \
  macsublayer/macreader.h                                   \
  macsublayer/testmacsublayer.h                             \
  llcsublayer/llcsublayer.h                                 \
  networklayer/types.h                                      \
  networklayer/networklayer.h                               \
  networklayer/neighbourinfo.h                              \
  networklayer/testnetworklayer.h                           \
  networklayer/routingprocess.h                             \
  networklayer/helloneighbourpacket.h                       \
  networklayer/networkreader.h                              \
  networklayer/connectionwrapper.h                          \
  networklayer/neighbourinfopacket.h                        \
  networklayer/routingtable.h                               \
  networklayer/routerinfo.h                                 \
  networklayer/neighbourinfoackpacket.h                     \
  networklayer/routingtableprocess.h                        \
  networklayer/ippacket.h                                   \
  router/router.h                                           \
  transportlayer/transportlayer.h                           \
  transportlayer/socket.h                                   \
  transportlayer/tcppacket.h                                \
  transportlayer/testtransportlayer.h                       \
  transportlayer/transportreader.h                          \
  transportlayer/listener.h                                 \
  host/host.h                                               \
  host/testhost.h                                           \

