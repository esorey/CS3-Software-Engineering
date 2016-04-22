all: server

server:
	g++ server.cpp config_parser.cc -std=c++0x -o server -lboost_system -lpthread

server-1.57: 
	clang++ server.cpp config_parser.cc -I /usr/local/Cellar/boost/1.57.0/include -L /usr/local/Cellar/boost/1.57.0/lib -std=c++11 -o server -lboost_system

server-1.60:
	clang++ server.cpp config_parser.cc -I /usr/local/Cellar/boost/1.60.0_1/include -L /usr/local/Cellar/boost/1.60.0_1/lib -std=c++11 -o server -lboost_system



clean:
	rm server
