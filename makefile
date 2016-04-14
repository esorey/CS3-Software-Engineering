all: server

server: asio_http.cpp
	clang++ asio_http.cpp -I /usr/local/Cellar/boost/1.57.0/include -L /usr/local/Cellar/boost/1.57.0/lib -std=c++11 -o server -lboost_system

test: test.cpp
	clang++ test.cpp -I /usr/local/Cellar/boost/1.57.0/include -L /usr/local/Cellar/boost/1.57.0/lib -std=c++11 -o test -lboost_system

clean:
	rm asio_http
