.SUFFIXES:
.SUFFIXES:      .cc .o .h

CXX			= g++
CXXFLAGS	= -O6 -Wall

OBJ = tcp_socket.o protocol.o thread.o

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all:	client_main server_main

clean:
	rm -f client_main server_main client_main.o server_main.o $(OBJ)

tar:
	@tar zcvf sockets.tgz Makefile *.h *.cc

client_main:	client_main.o $(OBJ)
	$(CXX) -o $@ client_main.o $(OBJ) -lpthread

server_main:	server_main.o $(OBJ)
	$(CXX) -o $@ server_main.o $(OBJ) -lpthread

client_main.o:	client_main.cc tcp_socket.h
server_main.o:	server_main.cc tcp_socket.h thread.h
tcp_socket.o:	tcp_socket.cc tcp_socket.h
