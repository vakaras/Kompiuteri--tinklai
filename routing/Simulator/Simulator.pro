QT                 += core testlib
QT                 -= gui
TARGET              = Simulator
CONFIG             += console
CONFIG             -= app_bundle
TEMPLATE            = app


QMAKE_CXXFLAGS     += -std=gnu++0x


SOURCES +=                                                            \
  src/main.cpp                                                        \
  src/unidirectionalcable/unidirectionalcable.cpp                     \
  src/unidirectionalcable/testunidirectionalcable.cpp                 \
  src/datalink/datalink.cpp                                           \
  src/datalink/testdatalink.cpp

HEADERS +=                                                            \
  src/types.h                                                         \
  src/interfaces/IPhysicalConnection.h                                \
  src/unidirectionalcable/unidirectionalcable.h                       \
  src/unidirectionalcable/testunidirectionalcable.h                   \
  src/interfaces/ILink.h                                              \
  src/datalink/datalink.h                                             \
  src/datalink/testdatalink.h                                         \
