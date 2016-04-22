// David Inglis & Eli Sorey
// server.cpp: a very simple http server
// Referenced the Boost TCP server tutorial: 
// http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/tutorial/tutdaytime2.html

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"

using boost::asio::ip::tcp;

const int max_length = 1024;

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

// Parses a port number from a config file
int parse_port(const char *config_string) {
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);

    if (config.statements_.at(0)->tokens_.at(0).compare("port") != 0) {
        std::cerr << "first line of config file must be port number" << std::endl;
        return -1;
    }
    return std::stoi(config.statements_.at(0)->tokens_.at(1));
}

// Handles incoming server requests according to provided parameters
// If echo is true, the server will echo the HTTP request it receives.
// Otherwise, it will serve "Hello, world!".
void handle_request(tcp::socket *sock, bool echo) {
    try {
        if (echo) {  // Echo the HTTP request
            char data[max_length];
            boost::system::error_code error;
            size_t length;
            for (;;) {
                char data[max_length];
                boost::system::error_code error;
                size_t length = sock->read_some(boost::asio::buffer(data), error);

                if (error == boost::asio::error::eof)
                  break; // Connection closed cleanly by peer.
                else if (error)
                  throw boost::system::system_error(error); // Some other error.

                // Echo the request back to the client
                boost::asio::write(*sock, boost::asio::buffer(data, length));
                break;
            }
        }

        else {  // Serve "Hello, world!"
            boost::asio::write(*sock, boost::asio::buffer(serve_page()));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: ./server [-e] <server_config_file> "  << std::endl;
        exit(1);
    }

    // Parse command-line args
    int i;
    int echo = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0)  // Check for echo parameter
        {
            echo = 1;  
        }
    }

    // Run the server
    try {
        boost::asio::io_service io_service;
        std::string config_file = std::string(argv[1]);
        int port = parse_port(argv[1]);
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
