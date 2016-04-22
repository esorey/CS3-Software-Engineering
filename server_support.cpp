#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"

using boost::asio::ip::tcp;

const int max_length = 1024;

// Serves a simple hello world HTML page
std::string serve_hello_page() {
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
            length = sock->read_some(boost::asio::buffer(data), error);

            if (error == boost::asio::error::eof)
              return; // Connection closed cleanly by peer.
            else if (error)
              throw boost::system::system_error(error); // Some other error.

            // Echo the request back to the client
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }

        else {  // Serve "Hello, world!"
            boost::asio::write(*sock, boost::asio::buffer(serve_hello_page()));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}