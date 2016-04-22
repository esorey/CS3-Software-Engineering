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
        std::cerr << "usage: ./server [-e] <server_config_file> "  << std::endl;
        exit(1);
    }

    // Parse command-line args
    int i;
    int echo = 0;
    char *config_file;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0)  // Check for echo parameter
        {
            echo = 1;  
        }
        else if (strncmp(argv[i], "-", 2) != 0) {
            config_file = argv[i];
        }
    }

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
            handle_request(&sock, echo);

        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
