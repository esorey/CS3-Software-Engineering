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
#include "request-handler.h"
#include "echo-request-handler.h"

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


    std::map<std::string, RequestHandler*> handlers;

    int port = -1;
    NginxConfigParser parser;
    NginxConfig config;
    parser.Parse(config_file, &config);
    for (int i = 0; i < config.statements_.size(); i++) {
        std::string type = config.statements_.at(i)->tokens_.at(0);
        if (type.compare("port") == 0) {
            if (port == -1) {
                port = std::stoi(config.statements_.at(0)->tokens_.at(1));
            }
            else {
                std::cerr << "Duplicate port declaration!" << std::endl;
                exit(1);
            }
        }
        else if (type.compare("handler") == 0) {
            std::string handler_type = config.statements_.at(i)->tokens_.at(1);
            std::map<std::string, std::string> config_map;
            for (int j = 0; j < config.statements_.at(i)->child_block_->statements_.size(); j++) {
                config_map.insert(std::pair<std::string, std::string>(config.statements_.at(i)->child_block_->statements_.at(j)->tokens_.at(0),
                                  config.statements_.at(i)->child_block_->statements_.at(j)->tokens_.at(1)));
            }

            std::cout << handler_type << std::endl;
            if (handler_type.compare("static") == 0) {

            }

            else if (handler_type.compare("echo") == 0) {
                EchoRequestHandler e;
                RequestHandler* h = &e;
                h->Init(config_map);
                handlers.insert(std::pair<std::string, RequestHandler*>(config_map["path"], h));
            }
        }

        else {
            std::cerr << "Unsupported type: " << type << std::endl;
            exit(1);
        }
    }
    

    for(auto elem : handlers)
    {
       std::cout << elem.first << "\n";
    }
    // Run the server
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

        // loop on accepting input and serve the page in response
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
