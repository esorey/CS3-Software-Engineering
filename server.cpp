// David Inglis & Eli Sorey
// server.cpp: a very simple http server
// Referenced the Boost TCP server tutorial: 
// http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/tutorial/tutdaytime2.html

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"

using boost::asio::ip::tcp;

// Serves a simple hello world HTML page
std::string serve_page() {
    std::stringstream ss;
    std::string body = "<html><body>Hello, world!</body></html>";
    ss << "HTTP/1.0 200 OK\n";
    ss << "content-type: text/html\n";
    ss << "content-length: " << body.length() << "\n\n";
    ss << body;
    return ss.str();
}

int parse_port(const char *config_string) {
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);
    return std::stoi(config.statements_.at(0)->tokens_.at(0));
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: ./server <server_config_file>"  << std::endl;
        return(1);
    }
    try
    {
        boost::asio::io_service io_service;
        std::string config_file = std::string(argv[1]);
        int port = parse_port(argv[1]);
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

        // loop on accepting input and serving the page in response
        while(true) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            boost::asio::write(socket, boost::asio::buffer(serve_page()));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}