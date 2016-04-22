GTEST_DIR=gtest-1.7.0

all: server

server:
	clang++ server.cpp config_parser.cc -std=c++11 -o server -lboost_system -lpthread

server-1.57: 
	clang++ server.cpp config_parser.cc -I /usr/local/Cellar/boost/1.57.0/include -L /usr/local/Cellar/boost/1.57.0/lib -std=c++11 -o server -lboost_system
	./server server_config

server-1.60:
	clang++ server.cpp config_parser.cc -I /usr/local/Cellar/boost/1.60.0_1/include -L /usr/local/Cellar/boost/1.60.0_1/lib -std=c++11 -o server -lboost_system

test:
	clang++ -std=c++11 -stdlib=libc++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	clang++ -std=c++11 -stdlib=libc++ -isystem ${GTEST_DIR}/include -pthread config_parser_test.cc config_parser.cc ${GTEST_DIR}/src/gtest_main.cc libgtest.a -o config_parser_test
	./config_parser_test


clean:
	rm server
