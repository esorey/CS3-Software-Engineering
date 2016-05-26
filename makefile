CC = temp
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC = g++
endif
ifeq ($(UNAME_S),Darwin)
	CC = clang++
endif

GTEST_DIR=gtest-1.7.0
GTEST_I = -isystem ${GTEST_DIR}/include -I${GTEST_DIR}
LDFLAGS = -static-libgcc -static-libstdc++ -pthread -Wl,-Bstatic -lboost_thread -lboost_system

all: server

libgtest.a: $(GTEST_DIR)/src/gtest-all.cc
	${CC} -std=c++11 -stdlib=libc++ ${GTEST_I} ${LDFLAGS} -c $^
	ar -rv libgtest.a gtest-all.o

gtest_main.o: $(GTEST_DIR)/src/gtest_main.cc
	${CC} -c $(GTEST_I) $(GTEST_DIR)/src/gtest_main.cc

config_parser.o: config_parser.cc config_parser.h
	${CC} -std=c++11 -g -c $<

config_parser_test.o: config_parser_test.cc config_parser.h
	${CC} -std=c++11 -g $(GTEST_I) ${LDFLAGS} -c config_parser_test.cc

config_parser_main.o: config_parser_main.cc config_parser.h
	${CC} -std=c++11 -g -c config_parser_main.cc

config_parser_test: config_parser.o config_parser_test.o libgtest.a gtest_main.o
	${CC} ${LDFLAGS} $^ -o config_parser_test

echo-request-handler.o: handlers/echo-request-handler.cpp
	${CC} -std=c++11 -g -c -o $@ $< $(I_BOOST)

file-request-handler.o: handlers/file-request-handler.cpp
	${CC} -std=c++11 -g -c -o $@ $< $(I_BOOST)

not-found-request-handler.o: handlers/not-found-request-handler.cpp
	${CC} -std=c++11 -g -c -o $@ $< $(I_BOOST)

server_support.o: server_support.cpp config_parser.o
	${CC} -std=c++11 -g -c -o $@ $< $(I_BOOST)

server.o: server.cpp server_support.o config_parser.o
	${CC} -std=c++11 -g -c -o  $@ $< $(I_BOOST)

server: echo-request-handler.o file-request-handler.o not-found-request-handler.o server.o config_parser.o server_support.o
	${CC} -g -std=c++11 -o $@ $^ $(I_BOOST) $(L_BOOST) ${LDFLAGS}

server_test.o: server_test.cc server.cpp
	${CC}-std=c++11 $(GTEST_I) -c -pthread $^ $^ $(I_BOOST)

server_test: file-request-handler.o not-found-request-handler.o echo-request-handler.o server_support.o config_parser.o server_test.o libgtest.a gtest_main.o
	${CC}-std=c++11 -o $@ $^ $(I_BOOST) $(L_BOOST) ${LDFLAGS}

test:
	make server_test
	make config_parser_test
	./server_test
	./config_parser_test

clean:
	rm -rf *.o server config_parser_test server_test



