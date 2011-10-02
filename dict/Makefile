FLAGS=--std=c++0x -g

all: server client

server: server.h server.cpp socket.h socket.cpp package.cpp package.h \
	dict.h
	g++ ${FLAGS} -o server server.cpp socket.cpp package.cpp

client: client.h client.cpp socket.h socket.cpp package.cpp package.h
	g++ ${FLAGS} -o client client.cpp socket.cpp package.cpp

clear:
	rm -rf server client
