FLAGS=--std=c++0x -g

all: mail

mail: main.cpp encoders.h utils.h mime.h mime.cpp encoders.cpp utils.cpp\
	sender.h socket.h socket.cpp sender.cpp
	g++ ${FLAGS} -o mail main.cpp mime.cpp encoders.cpp utils.cpp \
		socket.cpp sender.cpp

clear:
	rm -rf mail
