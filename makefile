all: server

server: server.cpp
	clang++ server.cpp -I /usr/local/Cellar/boost/1.57.0/include -L /usr/local/Cellar/boost/1.57.0/lib -std=c++11 -o server -lboost_system

clean:
	rm server
