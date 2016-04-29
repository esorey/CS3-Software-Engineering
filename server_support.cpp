#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"
#include "request.h"
#include "echo_request_handler.h"
#include "file_request_handler.h"

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
        request r;
        boost::system::error_code error;
        r.length = sock->read_some(boost::asio::buffer(r.data), error);

        if (error == boost::asio::error::eof) {
            printf("EOF error \n");
            return;
        }

        else if (error) {
            printf("Some other error \n");
            throw boost::system::system_error(error);
        }
        
        if (echo) {  // Echo the HTTP request
            echo_request_handler e(r, sock);
            e.handle();
        }

        else {  // Serve "Hello, world!"
            file_request_handler f(r, sock, "str");
            f.handle();
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}