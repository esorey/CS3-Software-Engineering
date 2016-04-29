// David Inglis & Eli Sorey
// server.cpp: a very simple http server
// Referenced the Boost TCP server tutorial: 
// http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/tutorial/tutdaytime2.html

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server.h"
#include "server_support.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: ./server <server_config_file> "  << std::endl;
        exit(1);
    }

    // Parse command-line args
    int i;
    int echo = 0;
    char *config_file = argv[1];

    // Run the server
    try {
        boost::asio::io_service io_service;
        int port = parse_port(config_file);
        if (port == -1) {
            return 1;
        }
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

        // loop on accepting input and serve the page in response
        while(true) {
            tcp::socket sock(io_service);
            acceptor.accept(sock);
            handle_request(&sock, config_file);

        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
