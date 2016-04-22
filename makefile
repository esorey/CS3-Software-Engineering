GTEST_DIR=gtest-1.7.0
GTEST_I = -isystem ${GTEST_DIR}/include -I${GTEST_DIR}

all: server

libgtest.a: $(GTEST_DIR)/src/gtest-all.cc
	clang++ -std=c++11 -stdlib=libc++ ${GTEST_I} -pthread -c $^
	ar -rv libgtest.a gtest-all.o

gtest_main.o: $(GTEST_DIR)/src/gtest_main.cc
	clang++ -c $(GTEST_I) $(GTEST_DIR)/src/gtest_main.cc

config_parser.o: config_parser.cc config_parser.h
	clang++ -std=c++11 -c $<

config_parser_test.o: config_parser_test.cc config_parser.h
	clang++ -std=c++11 $(GTEST_I) -pthread -c config_parser_test.cc

config_parser_main.o: config_parser_main.cc config_parser.h
	clang++ -std=c++11 -c config_parser_main.cc

config_parser_test: config_parser.o config_parser_test.o libgtest.a gtest_main.o
	clang++ -pthread $^ -o config_parser_test

server_support.o: server_support.cpp config_parser.o
	clang++ -std=c++11 -c -o $@ $< $(I_BOOST)

server.o: server.cpp server_support.o config_parser.o
	clang++ -std=c++11 -c -o  $@ $< $(I_BOOST)

server: server.o config_parser.o server_support.o
	clang++ -std=c++11 -o $@ $^ $(I_BOOST) $(L_BOOST) -lboost_system -lpthread

server_test.o: server_test.cc server.h
	clang++ -std=c++11 $(GTEST_I) -c -pthread $^ $^ $(I_BOOST)

server_test: server_support.o config_parser.o server_test.o libgtest.a gtest_main.o
	clang++ -std=c++11 -o $@ $^ $(I_BOOST) $(L_BOOST) -lboost_system -lpthread
	./server_test

clean:
	rm *.o server config_parser_test server server_test



