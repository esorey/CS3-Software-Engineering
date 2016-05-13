// David Inglis & Eli Sorey
// server.cpp: a very simple http server
// Referenced the Boost TCP server tutorial: 
// http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/tutorial/tutdaytime2.html

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: ./server <server_config_file> "  << std::endl;
        exit(1);
    }

    // Set up config details from input config file
    char *config_file = argv[1];
    std::map<std::string, std::shared_ptr<RequestHandler>> handlers;
    int port = -1;
    parse_config(&port, &handlers, config_file);
    
    // Run the server
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

        // loop on handling incoming requests
        while(true) {
            tcp::socket sock(io_service);
            acceptor.accept(sock);
            handle_request(&sock, handlers);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
